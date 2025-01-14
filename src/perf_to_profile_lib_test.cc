/*
 * Copyright (c) 2018, Google Inc.
 * All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/perf_to_profile_lib.h"
#include "src/quipper/base/logging.h"
#include "src/compat/test_compat.h"

namespace {

using ::testing::Eq;

TEST(PerfToProfileTest, ParseArguments) {
  struct Test {
    string desc;
    std::vector<const char*> argv;
    string expectedInput;
    string expectedOutput;
    bool expectedOverwriteOutput;
    bool wantError;
  };

  std::vector<Test> tests;
  tests.push_back(Test{
      .desc = "With input, output and overwrite flags",
      .argv = {"<exec>", "-i", "input_perf_file", "-o", "output_profile", "-f"},
      .expectedInput = "input_perf_file",
      .expectedOutput = "output_profile",
      .expectedOverwriteOutput = true,
      .wantError = false,
  });
  tests.push_back(Test{
      .desc = "With input and output flags",
      .argv = {"<exec>", "-i", "input_perf_file", "-o", "output_profile"},
      .expectedInput = "input_perf_file",
      .expectedOutput = "output_profile",
      .expectedOverwriteOutput = false,
      .wantError = false,
  });
  tests.push_back(Test{
      .desc = "With only overwrite flag",
      .argv = {"<exec>", "-f"},
      .expectedInput = "",
      .expectedOutput = "",
      .expectedOverwriteOutput = false,
      .wantError = true,
  });
  tests.push_back(Test{
      .desc = "With input, output, and invalid flags",
      .argv = {"<exec>", "-i", "input_perf_file", "-o", "output_profile", "-F"},
      .expectedInput = "",
      .expectedOutput = "",
      .expectedOverwriteOutput = false,
      .wantError = true,
  });
  tests.push_back(Test{
      .desc = "With an invalid flag",
      .argv = {"<exec>", "-F"},
      .expectedInput = "",
      .expectedOutput = "",
      .expectedOverwriteOutput = false,
      .wantError = true,
  });
  for (auto test : tests) {
    string input;
    string output;
    bool overwriteOutput;
    LOG(INFO) << "Testing: " << test.desc;
    EXPECT_THAT(ParseArguments(test.argv.size(), test.argv.data(), &input,
                               &output, &overwriteOutput),
                Eq(!test.wantError));
    if (!test.wantError) {
      EXPECT_THAT(input, Eq(test.expectedInput));
      EXPECT_THAT(output, Eq(test.expectedOutput));
      EXPECT_THAT(overwriteOutput, Eq(test.expectedOverwriteOutput));
    }
    optind = 1;
  }
}

// Assumes relpath does not begin with a '/'
string GetResource(const string& relpath) {
  return "src/" + relpath;
}

TEST(PerfToProfileTest, RawPerfDataStringToProfiles) {
  string raw_perf_data_path(
      GetResource("testdata"
                  "/multi-event-single-process.perf.data"));
  const auto profiles = StringToProfiles(ReadFileToString(raw_perf_data_path));
  EXPECT_EQ(profiles.size(), 1);
}

TEST(PerfToProfileTest, PerfDataProtoStringToProfiles) {
  string perf_data_proto_path(
      GetResource("testdata"
                  "/multi-event-single-process.perf_data.pb.data"));
  const auto profiles =
      StringToProfiles(ReadFileToString(perf_data_proto_path));
  EXPECT_EQ(profiles.size(), 1);
}

}  // namespace

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
