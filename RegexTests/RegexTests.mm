//
//  RegexTests.m
//  RegexTests
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <Regex/Regex.h>

@interface RegexTests : XCTestCase

@end

@implementation RegexTests

- (void)testExample {
    bool success;
    Regex::JIT jit(Regex::jit("01*1(A|B)", success));
    if (!success)
        XCTFail();

    Regex::Machine &m(jit);
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
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
