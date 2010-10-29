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

var DrillbitReporter = function (output) {
    this.output = output;
    this.specReports = [];
}

DrillbitReporter.prototype.send = function (specReport) {
    this.output(JSON.stringify(specReport));
}

DrillbitReporter.prototype.reportSpecResults = function (spec) {
    var results = spec.results(),
        items = results.getItems(),
        expectations = [];

    items.forEach(function (item) {
         if (item.type != 'expect') return;
         expectations.push({
             message: item.message,
             matcher: item.matcherName,
             expected: item.expected,
             actual: item.actual,
             passed: item.passed()
         });
    });

    this.specReports.push({
        description: spec.description,
        suite: spec.suite.description,
        passCount: results.passCount,
        failCount: results.failCount,
        expectations: expectations
    });
}

DrillbitReporter.prototype.reportRunnerResults = function () {
    this.output(JSON.stringify(this.specReports));
}
