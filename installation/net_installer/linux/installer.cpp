/** * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#include "installer.h"
#include "titanium_icon.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>

#define LICENSE_WINDOW_WIDTH 600
#define LICENSE_WINDOW_HEIGHT 500
#define NO_LICENSE_WINDOW_WIDTH 400 
#define NO_LICENSE_WINDOW_HEIGHT 150
#define PROGRESS_WINDOW_WIDTH 350 
#define PROGRESS_WINDOW_HEIGHT 130
#define BADGE_MAX_DIMENSION 100
#define ICON_MAX_DIMENSION 30

#define UNKNOWN_INSTALL 0
#define HOMEDIR_INSTALL 1
#define SYSTEM_INSTALL 2

void *download_thread_f(gpointer data);
void *install_thread_f(gpointer data);
static gboolean watcher(gpointer data);
static void install_cb(GtkWidget *widget, gpointer data);
static void cancel_cb(GtkWidget *widget, gpointer data);
static void destroy_cb(GtkWidget *widget, gpointer data);
static int do_install_sudo();
bool can_write_to_all_files(string);

static gchar* application_path = NULL;
static gchar* update_filename = NULL;
static gchar* install_type;
static gchar** urls = NULL;
static gboolean in_sudo_mode;
static GOptionEntry option_entries[] =
{
	{ "apppath", 0, 0, G_OPTION_ARG_STRING, &application_path, "The application path", NULL},
	{ "updatefile", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_FILENAME, &update_filename, "The filename of the update", NULL},
	{ "type", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_STRING, &install_type, "Force installation type -- non-interactive", NULL},
	{ "sudo", 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_NONE, &in_sudo_mode, "Whether or not the installer is running inside a sudoed environment", NULL},
	{ G_OPTION_REMAINING, 0, G_OPTION_FLAG_OPTIONAL_ARG, G_OPTION_ARG_STRING_ARRAY, &urls, "Url or filename of a component to install", NULL},
	{ NULL }
};

void show_error(string error, GtkWidget* window = NULL);

void show_error(string error, GtkWidget* window)
{
	GtkWidget* dialog = gtk_message_dialog_new(
		GTK_WINDOW(window),
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		"%s",
		error.c_str());
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

Installer* Installer::instance;
string Installer::applicationPath;
string Installer::systemRuntimeHome;
string Installer::userRuntimeHome;
string Installer::updateFilename;
vector<string> originalArgs;

Installer::Installer(vector<Job*> jobs, int installType) :
	jobs(jobs),
	app(NULL),
	installType(installType),
	stage(PREDOWNLOAD),
	currentJob(NULL),
	error("")
{
	Installer::instance = this;

	bool isUpdate = !updateFilename.empty();
	if (isUpdate && !can_write_to_all_files(applicationPath))
	{
		this->SetStage(SUDO_REQUEST);
		return;
	}
	else if (isUpdate)
	{
		this->app = Application::NewApplication(updateFilename, applicationPath);
	} 
	else
	{
		this->app = Application::NewApplication(applicationPath);
	}

	this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GdkPixbuf* applicationIcon = NULL;
	if (!app->image.empty())
	{
		applicationIcon = this->GetApplicationPixbuf(ICON_MAX_DIMENSION);
		gtk_window_set_icon(GTK_WINDOW(this->window), applicationIcon);
	}
	if (!applicationIcon)
	{
		GdkPixbuf* titanium_icon = gdk_pixbuf_new_from_xpm_data(titanium_xpm);
		gtk_window_set_icon(GTK_WINDOW(this->window), titanium_icon);
	}

	string title = this->app->name + " Installer";
	gtk_window_set_title(GTK_WINDOW(this->window), title.c_str());
	g_signal_connect(
		G_OBJECT(this->window),
		"destroy",
		G_CALLBACK(destroy_cb),
		(gpointer) this);

	if (this->installType == UNKNOWN_INSTALL)
	{
		this->CreateIntroView();
	}
	else // If we know the install type, skip the intro
	{
		this->StartDownloading();
	}

	gdk_threads_enter();
	int timer = g_timeout_add(100, watcher, this);
	gtk_main();
	g_source_remove(timer);
	gdk_threads_leave();
}

void Installer::Finish()
{
	if (this->stage == SUCCESS)
	{
		if (!in_sudo_mode)
		{
			FILE *file;
			string dataPath = app->GetDataPath();
			string path = FileUtils::Join(dataPath.c_str(), ".installed", NULL);
			file = fopen(path.c_str(),"w"); 
			fprintf(file, "\n");
			fclose(file);
		}

		if (!updateFilename.empty() && FileUtils::IsFile(updateFilename))
		{
			unlink(updateFilename.c_str());
		}
	}
}

Installer::~Installer()
{
	std::vector<Job*>::iterator i = jobs.begin();
	while (i != jobs.end())
	{
		Job* j = *i;
		i = jobs.erase(i);
		delete j;
	}
}

void Installer::ResizeWindow(int width, int height)
{
	// Try very hard to center the window
	gtk_window_set_default_size(GTK_WINDOW(this->window), width, height);
	gtk_window_resize(GTK_WINDOW(this->window), width, height);
	gtk_window_set_gravity(GTK_WINDOW(this->window), GDK_GRAVITY_CENTER);
	gtk_window_move(
		GTK_WINDOW(this->window),
		gdk_screen_width()/2 - width/2,
		gdk_screen_height()/2 - height/2);
}

void Installer::CreateInfoBox(GtkWidget* vbox)
{
	// Create the top part with the application icon and information
	GtkWidget* infoVbox = gtk_vbox_new(FALSE, 2);

	string nameLabelText = "<span size=\"xx-large\">";
	nameLabelText.append(this->app->name);
	nameLabelText.append("</span>");
	GtkWidget* nameLabel = gtk_label_new(nameLabelText.c_str());
	gtk_label_set_use_markup(GTK_LABEL(nameLabel), TRUE);
	gtk_misc_set_alignment(GTK_MISC(nameLabel), 0.0, 0.0);
	gtk_box_pack_start(GTK_BOX(infoVbox), nameLabel, FALSE, FALSE, 0);
	if (!this->app->version.empty())
	{
		string versionLabelText = string("Version: ") + this->app->version;
		if (!updateFilename.empty())
			versionLabelText += "<span style=\"italic\"> (Update)</span>";
		GtkWidget* versionLabel = gtk_label_new(versionLabelText.c_str());
		gtk_label_set_use_markup(GTK_LABEL(versionLabel), TRUE);
		gtk_misc_set_alignment(GTK_MISC(versionLabel), 0.0, 0.0);
		gtk_box_pack_start(GTK_BOX(infoVbox), versionLabel, FALSE, FALSE, 0);
	}
	if (!this->app->publisher.empty())
	{
		string publisherLabelText = string("Publisher: ") + this->app->publisher;
		GtkWidget* publisherLabel = gtk_label_new(publisherLabelText.c_str());
		gtk_misc_set_alignment(GTK_MISC(publisherLabel), 0.0, 0.0);
		gtk_box_pack_start(GTK_BOX(infoVbox), publisherLabel, FALSE, FALSE, 0);
	}
	if (!this->app->url.empty())
	{
		string urlLabelText = string("From: ") + this->app->url;
		GtkWidget* urlLabel = gtk_label_new(urlLabelText.c_str());
		gtk_misc_set_alignment(GTK_MISC(urlLabel), 0.0, 0.0);
		gtk_box_pack_start(GTK_BOX(infoVbox), urlLabel, FALSE, FALSE, 0);
	}

	GtkWidget* icon = this->GetApplicationIcon();
	GtkWidget* infoBox = gtk_hbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(infoBox), 5);
	gtk_box_pack_start(GTK_BOX(infoBox), icon, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(infoBox), infoVbox, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), infoBox, FALSE, FALSE, 0);

	GtkWidget* hseparator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), hseparator, FALSE, FALSE, 5);
}

void Installer::CreateIntroView()
{
	int width, height;
	std::string licenseText = this->app->GetLicenseText();
	if (licenseText.empty())
	{
		width = NO_LICENSE_WINDOW_WIDTH;
		height = NO_LICENSE_WINDOW_HEIGHT;
	}
	else
	{
		width = LICENSE_WINDOW_WIDTH;
		height = LICENSE_WINDOW_HEIGHT;
	}

	GtkWidget* windowVbox = gtk_vbox_new(FALSE, 0);
	this->CreateInfoBox(windowVbox);

	// Create the part with the license
	if (!licenseText.empty())
	{
		GtkWidget* licenseFrame = gtk_frame_new(
			"By continuing, you agree to the following terms "
			"and conditions on use of this software");

		GtkWidget* licenseTextView = gtk_text_view_new();
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(licenseTextView), GTK_WRAP_WORD);
		gtk_text_buffer_set_text(
			gtk_text_view_get_buffer(GTK_TEXT_VIEW(licenseTextView)),
			licenseText.c_str(), -1);
		GtkWidget* scroller = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy (
			GTK_SCROLLED_WINDOW(scroller),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport(
			GTK_SCROLLED_WINDOW(scroller), licenseTextView);
		gtk_container_set_border_width(GTK_CONTAINER(scroller), 5);

		gtk_container_set_border_width(GTK_CONTAINER(licenseFrame), 5);
		gtk_container_add(GTK_CONTAINER(licenseFrame), scroller);
		gtk_box_pack_start(GTK_BOX(windowVbox), licenseFrame, TRUE, TRUE, 0);

		GtkWidget* hseperator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(windowVbox), hseperator, FALSE, FALSE, 5);
	}

	if (this->jobs.size() > 0)
	{
		// Install dialog label
		GtkWidget* label = gtk_label_new(
			"This application may need to download and install "
			"additional components. Where should they be installed?");
		gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
		gtk_widget_set_size_request(label, width - 5, -1);
		gtk_misc_set_alignment(GTK_MISC(label), 0.0, 1.0);

		// Install type combobox
		GtkListStore* store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
		GtkTreeIter iter;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
			0, GTK_STOCK_HOME,
			1, "Install to my home directory", -1);
		std::string text = std::string("Install to ") + systemRuntimeHome;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
			0, GTK_STOCK_DIALOG_AUTHENTICATION,
			1, text.c_str(), -1);
		this->installCombo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));

		GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
		gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(installCombo), renderer, FALSE);
		gtk_cell_layout_set_attributes(
			GTK_CELL_LAYOUT(installCombo), renderer,
			"stock-id", 0, NULL);
		renderer = gtk_cell_renderer_text_new();
		gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(installCombo), renderer, TRUE);
		gtk_cell_layout_set_attributes(
			GTK_CELL_LAYOUT(installCombo), renderer,
			"text", 1, NULL);
		gtk_combo_box_set_active(GTK_COMBO_BOX(installCombo), 0);
		gtk_widget_set_size_request(installCombo, width - 5, -1);

		/* Pack label and combobox into vbox */
		GtkWidget* installTypeBox = gtk_vbox_new(FALSE, 0);
		gtk_container_set_border_width(GTK_CONTAINER(installTypeBox), 5);
		gtk_box_pack_start(GTK_BOX(installTypeBox), label, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(installTypeBox), installCombo, FALSE, FALSE, 10);
		gtk_box_pack_start(GTK_BOX(windowVbox), installTypeBox, FALSE, FALSE, 0);
	}
	else
	{
		// Make the window a little smaller in this case
		this->ResizeWindow(width, height - 40);
	}

	// Add the security warning
	GtkWidget* securityBox = gtk_hbox_new(FALSE, 0);
	GtkWidget* securityImage = gtk_image_new_from_stock(
		GTK_STOCK_DIALOG_WARNING,
		GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_box_pack_start(GTK_BOX(securityBox), securityImage, FALSE, FALSE, 3);

	GtkWidget* securityLabel = gtk_label_new(
		"<span style=\"italic\">"
		"This application has the same security access as "
		"all installed applications running on the desktop."
		"</span>");
	gtk_label_set_line_wrap(GTK_LABEL(securityLabel), TRUE);
	gtk_widget_set_size_request(securityLabel, width - 10, -1);
	gtk_label_set_use_markup(GTK_LABEL(securityLabel), TRUE);
	gtk_misc_set_alignment(GTK_MISC(securityLabel), 0.0, 0.5);
	gtk_box_pack_start(GTK_BOX(securityBox), securityLabel, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(securityBox), 5);
	gtk_box_pack_start(GTK_BOX(windowVbox), securityBox, FALSE, FALSE, 0);

	// Add the buttons
	string continueText = "Install";
	if (this->jobs.size() == 0)
		continueText = "Run";

	GtkWidget* cancel = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	GtkWidget* install = gtk_button_new_with_label(continueText.c_str());
	GtkWidget* install_icon = gtk_image_new_from_stock(
		GTK_STOCK_OK,
		GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(install), install_icon);
	GtkWidget* buttonBox = gtk_hbutton_box_new();
	gtk_container_set_border_width(GTK_CONTAINER(buttonBox), 5);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(buttonBox), 5);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonBox), GTK_BUTTONBOX_END);
	gtk_box_pack_start(GTK_BOX(buttonBox), cancel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), install, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(windowVbox), buttonBox, FALSE, FALSE, 0);

	g_signal_connect (
		G_OBJECT(cancel),
		"clicked",
		G_CALLBACK(cancel_cb),
		(gpointer) this);

	g_signal_connect (
		G_OBJECT(install),
		"clicked",
		G_CALLBACK(install_cb),
		(gpointer) this);

	gtk_container_add(GTK_CONTAINER(this->window), windowVbox);

	this->ResizeWindow(width, height);
	gtk_widget_show_all(this->window);
}

void Installer::CreateProgressView()
{
	// Remove all children from the window
	GList* children = gtk_container_get_children(GTK_CONTAINER(this->window));
	for (size_t i = 0; i < g_list_length(children); i++)
	{
		GtkWidget* w = (GtkWidget*) g_list_nth_data(children, i);
		gtk_container_remove(GTK_CONTAINER(this->window), w);
	}
	gtk_container_set_border_width(GTK_CONTAINER(this->window), 5);

	GtkWidget* vbox = gtk_vbox_new(FALSE, 5);
	this->CreateInfoBox(vbox);

	this->downloadingLabel = gtk_label_new("Downloading packages...");
	gtk_box_pack_start(GTK_BOX(vbox), this->downloadingLabel, FALSE, FALSE, 2);

	this->progressBar = gtk_progress_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), this->progressBar, FALSE, FALSE, 0);

	GtkWidget* hbox2 = gtk_hbox_new(FALSE, 0);
	GtkWidget* cancel_but = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox2), cancel_but, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 5);

	gtk_container_add(GTK_CONTAINER(this->window), vbox);

	g_signal_connect (
		G_OBJECT(cancel_but),
		"clicked",
		G_CALLBACK(cancel_cb),
		(gpointer) this);

	this->ResizeWindow(PROGRESS_WINDOW_WIDTH, PROGRESS_WINDOW_HEIGHT);
	gtk_widget_show_all(this->window);
}

GtkWidget* Installer::GetTitaniumIcon()
{
	GdkColormap* colormap = gtk_widget_get_colormap(this->window);
	GdkBitmap *mask = NULL;
	GdkPixmap* pixmap = gdk_pixmap_colormap_create_from_xpm_d(
		NULL,
		colormap,
		&mask,
		NULL,
		(gchar**) titanium_xpm);
	return gtk_image_new_from_pixmap(pixmap, mask);

}

GdkPixbuf* Installer::GetApplicationPixbuf(int maxDimension)
{
	GdkPixbuf *iconPixbuf = gdk_pixbuf_new_from_file(this->app->image.c_str(), NULL);
	if (iconPixbuf == NULL)
	{
		return NULL;
	}

	// If the image is hugenormous, try to scale it down to a reasonable size
	double scale = 1.0;
	int width = gdk_pixbuf_get_width(iconPixbuf);
	int height = gdk_pixbuf_get_height(iconPixbuf);
	if (width >= height && width > maxDimension)
	{
		scale = ((double) maxDimension) / ((double) width);
	}
	else if (height > width && height > maxDimension)
	{
		scale = ((double) maxDimension) / ((double) height);
	}

	if (scale != 1.0)
	{
		width = scale * width;
		height = scale * height;
		iconPixbuf = gdk_pixbuf_scale_simple(
			iconPixbuf, width, height, GDK_INTERP_BILINEAR);
	}
	return iconPixbuf;

}

GtkWidget* Installer::GetApplicationIcon()
{
	GdkPixbuf* appPixbuf;
	if (this->app->image != ""
		&& ((appPixbuf = this->GetApplicationPixbuf(BADGE_MAX_DIMENSION))))
	{
		return gtk_image_new_from_pixbuf(appPixbuf);
	}
	else // Use default Titanium icon
	{
		return this->GetTitaniumIcon();
	}
}

void Installer::StartInstallProcess()
{
	int choice = gtk_combo_box_get_active(GTK_COMBO_BOX(this->installCombo));
	if (choice == 0)
		this->installType = HOMEDIR_INSTALL;
	else
		this->installType = SYSTEM_INSTALL;

	// Home directory install or we already have a root-level effective userid
	if (this->installType == HOMEDIR_INSTALL || geteuid() == 0)
	{
		this->StartDownloading();
	}
	else
	{
		this->SetStage(SUDO_REQUEST);
		gtk_widget_hide(this->window);
		gtk_main_quit();
	}

}

void Installer::StartDownloading()
{
	if (!updateFilename.empty())
	{
		string updateURL = this->app->GetUpdateURL();
		jobs.push_back(new Job(updateURL, APPLICATION_JOB));
	}

	if (this->jobs.size() <= 0)
	{
		this->SetStage(SUCCESS);
		gtk_main_quit();
		return;
	}
	else
	{
		this->CreateProgressView();
		this->SetStage(DOWNLOADING);

		if (!g_thread_supported())
			g_thread_init(NULL);

		this->download_thread =
			g_thread_create(&download_thread_f, this, TRUE, NULL);

		if (this->download_thread == NULL)
			g_warning("Can't create download thread!\n");
	}

}

void Installer::UpdateProgress()
{
	Job *j = this->CurrentJob();

	if (this->window == NULL)
		return;

	Stage s = this->GetStage();
	if (j != NULL && (s == DOWNLOADING || s == INSTALLING))
	{
		double progress = j->GetProgress();
		gtk_progress_bar_set_fraction(
			GTK_PROGRESS_BAR(this->progressBar),
			progress);

		std::ostringstream text;
		if (s == INSTALLING)
			text << "Installing ";
		else
			text << "Downloading ";
		text << "package " << j->Index() << " of " << Job::total;

		gtk_label_set_text(GTK_LABEL(this->downloadingLabel), text.str().c_str());

	}
	else if (s == PREINSTALL)
	{
		gtk_progress_bar_set_fraction(
			GTK_PROGRESS_BAR(this->progressBar),
			1.0);
	}
	
}

static gboolean watcher(gpointer data)
{
	Installer::Stage s = Installer::instance->GetStage();
	if (s == Installer::PREINSTALL)
	{
		Installer::instance->StartInstalling();
	}
	else if (s == Installer::ERROR)
	{
		Installer::instance->ShowError();
		gtk_main_quit();
		return FALSE;
	}
	else if (s == Installer::CANCELLED)
	{
		gtk_main_quit();
		return FALSE;
	}
	else if (s == Installer::SUCCESS)
	{
		gtk_main_quit();
		return FALSE;
	}
	else
	{
		Installer::instance->UpdateProgress();
	}

	return TRUE;
}

void Installer::StartInstalling()
{
	if (this->installType == HOMEDIR_INSTALL)
		Job::installDirectory = userRuntimeHome;
	else if (this->installType == SYSTEM_INSTALL)
		Job::installDirectory = systemRuntimeHome;

	if (this->download_thread != NULL)
	{
		g_thread_join(this->download_thread);
		this->download_thread = NULL;

		// This has to happen when no other threads are
		// running, since CURL's global shutdown is not
		// thread safe and we need access to downloaded
		// files now.
		Job::ShutdownDownloader();

		if(!g_thread_create(&install_thread_f, this, FALSE, NULL))
			g_warning("Can't create install thread!\n");

		this->SetStage(INSTALLING);
	}
}


void *download_thread_f(gpointer data)
{
	Installer* inst = (Installer*) data;
	std::vector<Job*>& jobs = inst->GetJobs();
	try
	{
		for (size_t i = 0; i < jobs.size(); i++)
		{
			Job *j = jobs.at(i);
			inst->SetCurrentJob(j);
			j->Fetch();

			// Wait for an unzip job to finish before actually cancelling
			if (Installer::instance->GetStage() == Installer::CANCEL_REQUEST)
			{
				Installer::instance->SetStage(Installer::CANCELLED);
				return NULL;
			}
		}
	}
	catch (std::exception& e)
	{
		std::string message = e.what();
		inst->SetError(message);
		return NULL;
	}
	catch (std::string& e)
	{
		inst->SetError(e);
		return NULL;
	}
	catch (...)
	{
		std::string message = "Unknown error";
		inst->SetError(message);
		return NULL;
	}

	inst->SetStage(Installer::PREINSTALL);
	return NULL;
}

void *install_thread_f(gpointer data)
{
	Installer* inst = (Installer*) data;
	std::vector<Job*>& jobs = Installer::instance->GetJobs();
	try
	{
		for (size_t i = 0; i < jobs.size(); i++)
		{
			Job *j = jobs.at(i);
			inst->SetCurrentJob(j);
			j->Unzip();

			// Wait for an unzip job to finish before actually cancelling
			if (inst->GetStage() == Installer::CANCEL_REQUEST)
			{
				inst->SetStage(Installer::CANCELLED);
				return NULL;
			}
		}
	}
	catch (std::exception& e)
	{
		std::string message = e.what();
		inst->SetError(message);
		return NULL;
	}
	catch (std::string& e)
	{
		inst->SetError(e);
		return NULL;
	}
	catch (...)
	{
		std::string message = "Unknown error";
		inst->SetError(message);
		return NULL;
	}

	inst->SetStage(Installer::SUCCESS);
	return NULL;
}

void Installer::ShowError()
{
	if (this->error != "")
	{
		show_error(this->error, this->window);
	}
}

static void install_cb(GtkWidget *widget, gpointer data)
{
	Installer::instance->StartInstallProcess();
}

static void cancel_cb(GtkWidget *widget, gpointer data)
{
	Installer::Stage s = Installer::instance->GetStage();
	if (s == Installer::PREDOWNLOAD)
	{
		gtk_main_quit();
		Installer::instance->SetStage(Installer::CANCELLED);
	}
	else
	{
		// Wait for the download or install thread to cancel us
		Installer::instance->SetStage(Installer::CANCEL_REQUEST);
	}
}

static void destroy_cb(GtkWidget *widget, gpointer data)
{
	Installer::Stage s = Installer::instance->GetStage();
	if (s == Installer::PREDOWNLOAD)
	{
		gtk_main_quit();
		Installer::instance->SetStage(Installer::CANCELLED);
	}
	else
	{
		// Wait for the download or install thread to cancel us
		Installer::instance->SetStage(Installer::CANCEL_REQUEST);
	}
	Installer::instance->SetWindow(NULL);
}

// TODO: Switch to PolicyKit
int do_install_sudo()
{
	// Copy all but the first command-line arg
	std::vector<std::string> args;

	// Preserve the current environment so that http_proxy
	// settings still work.
	args.push_back("-k"); 
	args.push_back("--description");
	args.push_back("Titanium Network Installer");

	args.push_back("--");
	args.push_back(originalArgs.at(0));
	args.push_back("--sudo");

	// If the user has already chosen an install-type force
	// non-interactive mode when we start up again.
	if (Installer::instance->GetType() == SYSTEM_INSTALL)
	{
		args.push_back("--type");
		args.push_back("system");
	}
	else if (Installer::instance->GetType() == HOMEDIR_INSTALL)
	{
		args.push_back("--type");
		args.push_back("homedir");
	}

	// Preserve all arguments
	for (size_t i = 1; i < originalArgs.size(); i++)
	{
		args.push_back(originalArgs.at(i));
	}

	// Restart in a sudoed environment
	std::string cmd = "gksudo";
	int r = FileUtils::RunAndWait(cmd, args);
	if (r == 127)
	{
		// Erase gksudo specific options
		args.erase(args.begin());
		args.erase(args.begin());
		args.erase(args.begin());
		cmd = std::string("kdesudo");
		args.insert(args.begin(), "The Titanium installer needs adminstrator privileges to run. Please enter your password.");
		args.insert(args.begin(), "--comment");
		args.insert(args.begin(), "-d");
		r = FileUtils::RunAndWait(cmd, args);
	}
	if (r == 127)
	{
		// Erase kdesudo specific option
		args.erase(args.begin());
		args.erase(args.begin());
		args.erase(args.begin());
		cmd = std::string("xterm");
		args.insert(args.begin(), "-E");
		args.insert(args.begin(), "sudo");
		args.insert(args.begin(), "-e");
		r = FileUtils::RunAndWait(cmd, args);
	}
	return r;
}

bool can_write_to_all_files(std::string path)
{
	vector<string> files;
	vector<string> dirs;
	FileUtils::ListDir(path, files);
	vector<string>::iterator iter = files.begin();
	while (iter != files.end())
	{
		string file = *iter++;
		string fullPath = FileUtils::Join(path.c_str(), file.c_str(), NULL);
		if (FileUtils::IsDirectory(fullPath))
			dirs.push_back(fullPath);

		if (eaccess(fullPath.c_str(), W_OK))
			return false;
	}

	iter = dirs.begin();
	while (iter != dirs.end())
	{
		string fullPath = *iter++;
		if (!can_write_to_all_files(fullPath.c_str()))
			return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		originalArgs.push_back(argv[i]);

	GError *error = NULL;
	GOptionContext *context;
	context = g_option_context_new("- Installer utility");
	g_option_context_set_help_enabled(context, TRUE);
	g_option_context_add_main_entries(context, option_entries, NULL);
	g_option_context_add_group(context, gtk_get_option_group(TRUE));

	bool args_good = g_option_context_parse(context, &argc, &argv, &error) != 0;

	gtk_init(&argc, &argv);
	if (!args_good || application_path == NULL)
	{
		string msg = "The installer was not given enough information to continue. ";
		if (!args_good) msg += error->message;
		show_error(msg);
		return 1;
	}

	Job::InitDownloader();
	Installer::applicationPath = application_path;
	Installer::userRuntimeHome = FileUtils::GetUserRuntimeHomeDirectory();
	Installer::systemRuntimeHome = FileUtils::GetSystemRuntimeHomeDirectory();

	if (update_filename == NULL)
		Installer::updateFilename == string();
	else
		Installer::updateFilename = update_filename;

	vector<Job*> jobs;
	if (urls != NULL)
	{
		for (int i = 0; urls[i] != NULL; i++)
			jobs.push_back(new Job(urls[i]));
	}

	if (install_type != NULL)
	{
		if (!strcmp("homedir", install_type))
			new Installer(jobs, HOMEDIR_INSTALL);

		else if (!strcmp("system", install_type))
			new Installer(jobs, SYSTEM_INSTALL);
	}
	else
	{
		new Installer(jobs, UNKNOWN_INSTALL);
	}

	int result = Installer::instance->GetStage();

	// The installer wants to run again, but in sudoed mode
	if (result == Installer::SUDO_REQUEST)
	{
		result = do_install_sudo();
		Installer::instance->SetStage((Installer::Stage) result);
	}

	Installer::instance->Finish();
	result = Installer::instance->GetStage();
	delete Installer::instance;

	Job::ShutdownDownloader();
	return result;
}
