#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include<sstream>


// change if you choose to use a different header name
#include "CampusCompass.h"

using namespace std;

// the syntax for defining a test is below. It is important for the name to be
// unique, but you can group multiple tests with [tags]. A test can have
// [multiple][tags] using that syntax.
TEST_CASE("Invalid Commands", "[validation]") {
  // instantiate any class members that you need to test here
 CampusCompass  c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");

  SECTION("Invalid Student ID - Too Short") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"John\" 123 1 1 COP3530");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Test 2: Invalid name with numbers
  SECTION("Invalid Name - Contains Numbers") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"John123\" 12345678 1 1 COP3530");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Test 3: Invalid class code format
  SECTION("Invalid Class Code Format") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"John\" 12345678 1 1 COP35");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Test 4: Too many classes (> 6)
  SECTION("Too Many Classes") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"John\" 12345678 1 7 COP3530 COP3502 MAC2311 PHY2048 CDA3101 ENC1101 CHM2045");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Test 5: Class doesn't exist
  SECTION("Non-existent Class") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"John\" 12345678 1 1 ABC1234");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }
}

  // anything that evaluates to false in a REQUIRE block will result in a
  // failing test
 // fix me!

  // all REQUIRE blocks must evaluate to true for the whole test to pass
// also fix me!



  // you can also use "sections" to share setup code between tests, for example:
  // each section runs the setup code independently to ensure that they don't
  // affect each other

// Tests 3 different edge cases
TEST_CASE("Edge Cases", "[edge]") {
  CampusCompass c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");

  // Edge Case 1: Remove student that doesn't exist
  SECTION("Remove Non-existent Student") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("remove 99999999");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Edge Case 2: Drop class from student who doesn't have it
  SECTION("Drop Class Student Doesn't Have") {
    c.ParseCommand("insert \"Alice\" 11111111 1 1 COP3530");
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("dropClass 11111111 MAC2311");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }

  // Edge Case 3: Insert duplicate student ID
  SECTION("Insert Duplicate Student") {
    c.ParseCommand("insert \"Bob\" 22222222 1 1 COP3530");
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("insert \"Bob2\" 22222222 1 1 MAC2311");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }
}

// You must write 5 unique, meaningful tests for credit on the testing section
// of this project!

// See the following for an example of how to easily test your output.
// Note that while this works, I recommend also creating plenty of unit tests for particular functions within your code.
// This pattern should only be used for final, end-to-end testing.

// This uses C++ "raw strings" and assumes your CampusCompass outputs a string with
//   the same thing you print.
TEST_CASE("Command testing", "[integrate]") {
  // the following is a "raw string" - you can write the exact input (without
  //   any indentation!) and it should work as expected
  // this is based on the input and output of the first public test case
  CampusCompass c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");

  stringstream buffer;
  streambuf* old = cout.rdbuf(buffer.rdbuf());
// Insert some test cases
  c.ParseCommand("insert \"Student A\" 10000001 1 1 COP3502");
  c.ParseCommand("insert \"Student B\" 10000002 1 1 COP3502");
  c.ParseCommand("insert \"Student C\" 10000003 1 2 COP3502 MAC2311");
  c.ParseCommand("dropClass 10000001 COP3502");
  c.ParseCommand("remove 10000001");
  c.ParseCommand("removeClass COP3502");

  cout.rdbuf(old);

  string expectedOutput = "successful\nsuccessful\nsuccessful\nsuccessful\nunsuccessful\n2\n";
  REQUIRE(buffer.str() == expectedOutput);
}

TEST_CASE("Test isConnected", "[connectivity]") {
  CampusCompass c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");
  SECTION("Locations are connected") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("isConnected 1 23");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "successful\n");
  }

  SECTION("Locations not connected after edge closure") {
    c.ParseCommand("toggleEdgesClosure 3 1 2 1 4 1 50");
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("isConnected 1 23");
    cout.rdbuf(old);
    REQUIRE(buffer.str() == "unsuccessful\n");
  }
}

TEST_CASE("Drop last class removes student", "[edge-case]") {
  CampusCompass c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");

  c.ParseCommand("insert \"Solo\" 77777777 1 1 COP3530");
  c.ParseCommand("dropClass 77777777 COP3530");

  // Try to drop another class - should fail because student is gone
  stringstream buffer;
  streambuf* old = cout.rdbuf(buffer.rdbuf());
  c.ParseCommand("dropClass 77777777 MAC2311");
  cout.rdbuf(old);
  REQUIRE(buffer.str() == "unsuccessful\n");
}

TEST_CASE("Shortest Path ", "[shortest]") {
  CampusCompass c;
  c.ParseCSV("../data/edges.csv", "../data/classes.csv");

  // Student A lives at loc 1.
  // COP3502 is at loc 2 the Direct Edge exists in this scenario
  c.ParseCommand("insert \"Student A\" 10000001 1 1 COP3502");

  SECTION("Basic Shortest Path") {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    c.ParseCommand("printShortestEdges 10000001");

    cout.rdbuf(old);
    // We expect "Total Time: X" (where X is positive), NOT -1.
    string output = buffer.str();
    REQUIRE(output.find("Total Time: -1") == string::npos); // Should NOT be -1
    REQUIRE(output.find("Name: Student A") != string::npos);
  }
}

  // somehow pass your input into your CampusCompass and parse it to call the
  // correct functions, for example:
  /*
  CampusCompass c;
  c.parseInput(input)
  // this would be some function that sends the output from your class into a string for use in testing
  actualOutput = c.getStringRepresentation()
  */


