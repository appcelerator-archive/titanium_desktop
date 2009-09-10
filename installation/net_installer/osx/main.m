/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
#ifdef DEBUG
	int i;
	for (i = 0; i < argc; i++){
		printf("Arg %d is: %s\n",i,argv[i]);
	}
#endif

	return NSApplicationMain(argc,  (const char **) argv);
}
