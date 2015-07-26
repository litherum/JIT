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

- (void)runMachinesWithRegex:(std::string)regex tests:(void(^)(Regex::Machine&))block {
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
    [self runMachinesWithRegex:"01*1(A|B)" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run("0110"));
        XCTAssertTrue(m.run("011A"));
        XCTAssertTrue(m.run("011B"));
        XCTAssertFalse(m.run("011AB"));
        XCTAssertTrue(m.run("0111A"));
        XCTAssertFalse(m.run("1"));
        XCTAssertTrue(m.run("01B"));
        XCTAssertFalse(m.run("00"));
        XCTAssertFalse(m.run("01101"));
        XCTAssertFalse(m.run("01111"));
    }];
}

- (void)testDot {
    [self runMachinesWithRegex:"." tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("0"));
        XCTAssertTrue(m.run("\["));
        XCTAssertFalse(m.run("ab"));
    }];
}

- (void)testAlternate {
    [self runMachinesWithRegex:"a|b" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertFalse(m.run("q"));
        XCTAssertFalse(m.run("ab"));
    }];
}

- (void)testConcatenate {
    [self runMachinesWithRegex:"ab" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run("a"));
        XCTAssertFalse(m.run("abc"));
        XCTAssertFalse(m.run("ac"));
        XCTAssertTrue(m.run("ab"));
    }];
}

- (void)testStar {
    [self runMachinesWithRegex:"a*" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("aa"));
        XCTAssertTrue(m.run("aaaaaaaaaaaaaaaa"));
        XCTAssertFalse(m.run("aaaaaaaaaaaaaaaab"));
        XCTAssertFalse(m.run("aaaaaabaaaaaaaaaa"));
        XCTAssertFalse(m.run("baaaaaaaaaaaaaaaa"));
    }];
}

- (void)testPlus {
    [self runMachinesWithRegex:"a+" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("aa"));
        XCTAssertTrue(m.run("aaaaaaaaaaaaaaaa"));
        XCTAssertFalse(m.run("aaaaaaaaaaaaaaaab"));
        XCTAssertFalse(m.run("aaaaaabaaaaaaaaaa"));
        XCTAssertFalse(m.run("baaaaaaaaaaaaaaaa"));
    }];
}

- (void)testOrderOfOperations {
    [self runMachinesWithRegex:"a(bc)" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("abc"));
    }];
    [self runMachinesWithRegex:"(ab)c" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("abc"));
    }];
    [self runMachinesWithRegex:"(ab)*" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run(""));
        XCTAssertTrue(m.run("ab"));
        XCTAssertTrue(m.run("ababab"));
        XCTAssertFalse(m.run("a"));
        XCTAssertFalse(m.run("ababa"));
    }];
    [self runMachinesWithRegex:"ab|cd" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run(""));
        XCTAssertTrue(m.run("ab"));
        XCTAssertTrue(m.run("cd"));
        XCTAssertFalse(m.run("a"));
        XCTAssertFalse(m.run("c"));
        XCTAssertFalse(m.run("abcd"));
    }];
    [self runMachinesWithRegex:"ab*" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("ab"));
        XCTAssertTrue(m.run("abbb"));
        XCTAssertFalse(m.run("abab"));
    }];
    [self runMachinesWithRegex:"a|b*" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertTrue(m.run("bbbb"));
        XCTAssertFalse(m.run("abab"));
    }];
    [self runMachinesWithRegex:"(a|b)*" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertTrue(m.run("abbabbbbbbaaabbbbbbbbbb"));
        XCTAssertFalse(m.run("q"));
    }];
    [self runMachinesWithRegex:"[ab]*" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run(""));
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertTrue(m.run("abbabbbbbbaaabbbbbbbbbb"));
        XCTAssertFalse(m.run("q"));
    }];
}

- (void)testEscape {
    [self runMachinesWithRegex:"\\." tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run("a"));
        XCTAssertTrue(m.run("."));
    }];
    [self runMachinesWithRegex:"\\[" tests:^(Regex::Machine &m) {
        XCTAssertFalse(m.run("a"));
        XCTAssertTrue(m.run("["));
    }];
}

- (void)testCharacterRange {
    [self runMachinesWithRegex:"[a]" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertFalse(m.run("ab"));
    }];
    [self runMachinesWithRegex:"[a-c]" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertTrue(m.run("c"));
        XCTAssertFalse(m.run("ab"));
        XCTAssertFalse(m.run("d"));
    }];
    [self runMachinesWithRegex:"[abc]" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("b"));
        XCTAssertTrue(m.run("c"));
        XCTAssertFalse(m.run("d"));
        XCTAssertFalse(m.run("ab"));
    }];
    [self runMachinesWithRegex:"[ac-e]" tests:^(Regex::Machine &m) {
        XCTAssertTrue(m.run("a"));
        XCTAssertTrue(m.run("c"));
        XCTAssertTrue(m.run("d"));
        XCTAssertTrue(m.run("e"));
        XCTAssertFalse(m.run("b"));
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
        jit.run(haystack);
    }];
}

- (void)testInterpreterPerformance {
    bool success;
    __block Regex::Interpreter interpreter(Regex::interpret(".*a.*", success));
    XCTAssertTrue(success);
    [self measureBlock:^{
        interpreter.run(haystack);
    }];
}
@end
