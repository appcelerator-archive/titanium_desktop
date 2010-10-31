/**
 * Copyright 2010 Appcelerator, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

Drillbit = {};

var arguments = Titanium.App.getArguments();
if (arguments.length < 2) {
    Titanium.API.error("Missing required commandline arguments!");
    Titanium.App.exit();
}

var testFilePath = arguments[0],
    outputFilePath = arguments[1];

function loadTest() {
    var testFile = Titanium.Filesystem.getFile(testFilePath);
    if (!testFile.exists()) {
        Titanium.API.error("Test file not found!");
        Titanium.App.exit();
    }

    var testContents = testFile.read().toString();
        testType = testFile.extension();

    if (testType == 'js') {
        eval(testContents);
    } else if (testType == 'html') {
        // TODO: implement
    } else {
        Titanium.API.error("Invalid test file extension! Must be js or html.");
        Titanium.App.exit();
    }

    Drillbit.testDirectory = testFile.parent();
}

function getOutputWriter() {
    var outputStream = Titanium.Filesystem.getFileStream(outputFilePath);

    if (!outputStream.open(Titanium.Filesystem.MODE_WRITE)) {
        Titanium.API.error("Unable to open output file for writing!");
        Titanium.App.exit();
    }

    Titanium.API.addEventListener(Titanium.EXIT, function () {
        outputStream.close();
    });

    return function (data) {
        outputStream.write(data);
    }
}

var reporter = new DrillbitReporter(getOutputWriter());
jasmine.getEnv().addReporter(reporter);

loadTest();
