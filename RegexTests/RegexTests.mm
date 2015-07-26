//
//  RegexTests.m
//  RegexTests
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <Regex/Regex.h>
#import <sstream>

@interface RegexTests : XCTestCase

@end

@implementation RegexTests

- (void)runMachinesWithRegex:(std::string)regex tests:(void(^)(const std::function<bool(const std::string&)>&))block {
    bool success;
    Regex::JIT jit(Regex::jit(regex, success));
    XCTAssertTrue(success);
    block(jit);

    Regex::Interpreter intepreter(Regex::interpret(regex, success));
    XCTAssertTrue(success);
    block(intepreter);
}

- (void)runCompilationFailure:(std::string)regex {
    bool success;
    Regex::JIT jit(Regex::jit(regex, success));
    XCTAssertFalse(success);

    Regex::Interpreter intepreter(Regex::interpret(regex, success));
    XCTAssertFalse(success);
}

- (void)testCompilationFailure {
    [self runCompilationFailure:"("];
    [self runCompilationFailure:"(a)b)"];
    [self runCompilationFailure:"["];
    [self runCompilationFailure:"[a[b]c"];
    [self runCompilationFailure:"*"];
    [self runCompilationFailure:"+"];
}

- (void)testExample {
    [self runMachinesWithRegex:"01*1(A|B)" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m("0110"));
        XCTAssertTrue(m("011A"));
        XCTAssertTrue(m("011B"));
        XCTAssertFalse(m("011AB"));
        XCTAssertTrue(m("0111A"));
        XCTAssertFalse(m("1"));
        XCTAssertTrue(m("01B"));
        XCTAssertFalse(m("00"));
        XCTAssertFalse(m("01101"));
        XCTAssertFalse(m("01111"));
    }];
}

- (void)testDot {
    [self runMachinesWithRegex:"." tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("0"));
        XCTAssertTrue(m("\["));
        XCTAssertFalse(m("ab"));
    }];
}

- (void)testAlternate {
    [self runMachinesWithRegex:"a|b" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertFalse(m("q"));
        XCTAssertFalse(m("ab"));
    }];
}

- (void)testConcatenate {
    [self runMachinesWithRegex:"ab" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m("a"));
        XCTAssertFalse(m("abc"));
        XCTAssertFalse(m("ac"));
        XCTAssertTrue(m("ab"));
    }];
}

- (void)testStar {
    [self runMachinesWithRegex:"a*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("aa"));
        XCTAssertTrue(m("aaaaaaaaaaaaaaaa"));
        XCTAssertFalse(m("aaaaaaaaaaaaaaaab"));
        XCTAssertFalse(m("aaaaaabaaaaaaaaaa"));
        XCTAssertFalse(m("baaaaaaaaaaaaaaaa"));
    }];
}

- (void)testPlus {
    [self runMachinesWithRegex:"a+" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("aa"));
        XCTAssertTrue(m("aaaaaaaaaaaaaaaa"));
        XCTAssertFalse(m("aaaaaaaaaaaaaaaab"));
        XCTAssertFalse(m("aaaaaabaaaaaaaaaa"));
        XCTAssertFalse(m("baaaaaaaaaaaaaaaa"));
    }];
}

- (void)testOrderOfOperations {
    [self runMachinesWithRegex:"a(bc)" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("abc"));
    }];
    [self runMachinesWithRegex:"(ab)c" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("abc"));
    }];
    [self runMachinesWithRegex:"(ab)*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m(""));
        XCTAssertTrue(m("ab"));
        XCTAssertTrue(m("ababab"));
        XCTAssertFalse(m("a"));
        XCTAssertFalse(m("ababa"));
    }];
    [self runMachinesWithRegex:"ab|cd" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m(""));
        XCTAssertTrue(m("ab"));
        XCTAssertTrue(m("cd"));
        XCTAssertFalse(m("a"));
        XCTAssertFalse(m("c"));
        XCTAssertFalse(m("abcd"));
    }];
    [self runMachinesWithRegex:"ab*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("ab"));
        XCTAssertTrue(m("abbb"));
        XCTAssertFalse(m("abab"));
    }];
    [self runMachinesWithRegex:"a|b*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertTrue(m("bbbb"));
        XCTAssertFalse(m("abab"));
    }];
    [self runMachinesWithRegex:"(a|b)*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertTrue(m("abbabbbbbbaaabbbbbbbbbb"));
        XCTAssertFalse(m("q"));
    }];
    [self runMachinesWithRegex:"[ab]*" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m(""));
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertTrue(m("abbabbbbbbaaabbbbbbbbbb"));
        XCTAssertFalse(m("q"));
    }];
}

- (void)testEscape {
    [self runMachinesWithRegex:"\\." tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m("a"));
        XCTAssertTrue(m("."));
    }];
    [self runMachinesWithRegex:"\\[" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertFalse(m("a"));
        XCTAssertTrue(m("["));
    }];
}

- (void)testCharacterRange {
    [self runMachinesWithRegex:"[a]" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertFalse(m("ab"));
    }];
    [self runMachinesWithRegex:"[a-c]" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertTrue(m("c"));
        XCTAssertFalse(m("ab"));
        XCTAssertFalse(m("d"));
    }];
    [self runMachinesWithRegex:"[abc]" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("b"));
        XCTAssertTrue(m("c"));
        XCTAssertFalse(m("d"));
        XCTAssertFalse(m("ab"));
    }];
    [self runMachinesWithRegex:"[ac-e]" tests:^(const std::function<bool(const std::string&)>& m) {
        XCTAssertTrue(m("a"));
        XCTAssertTrue(m("c"));
        XCTAssertTrue(m("d"));
        XCTAssertTrue(m("e"));
        XCTAssertFalse(m("b"));
    }];
}
@end

@interface RegexPerformanceTests : XCTestCase

@end

@implementation RegexPerformanceTests {
    std::string haystack;
}

- (void)setUp {
    std::ostringstream ss;
    for (int i = 0; i < 1000000; ++i)
        ss << "sdfg";
    haystack = ss.str();
}

- (void)testJITPerformance {
    bool success;
    __block Regex::JIT jit(Regex::jit(".*a.*", success));
    XCTAssertTrue(success);
    [self measureBlock:^{
        jit(haystack);
    }];
}

- (void)testInterpreterPerformance {
    bool success;
    __block Regex::Interpreter interpreter(Regex::interpret(".*a.*", success));
    XCTAssertTrue(success);
    [self measureBlock:^{
        interpreter(haystack);
    }];
}
@end