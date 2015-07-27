//
//  JIT.cpp
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#include <sys/mman.h>

#include <cassert>
#include <string>
#include <unordered_set>
#include <vector>

#include "JIT.h"

#include "DFA.h"

#if !defined(__x86_64__) || !defined(__APPLE__)
#error Requires x86_64 on OS X!
#endif

namespace Regex {

class CodeGenerator {
public:
    CodeGenerator(const DFA& dfa): stateLocations(dfa.size()) {
        compileMachineCode(dfa);
        linkMachineCode(dfa);
    }
    
    void compileMachineCode(const DFA& dfa) {
        machineCode.push_back(PUSH_RBP);
        
        machineCode.push_back(REX_000);
        machineCode.push_back(MOV_RR);
        machineCode.push_back(0xe5); // stack pointer into base pointer
        
        machineCode.push_back(PUSH_RDI); // "this" ptr
        
        dfa.iterateNodes([&](DFANode node) {
            stateLocations[node] = machineCode.size();
            emitNextCharCall();
            machineCode.push_back(CMP_AL);
            machineCode.push_back(0);
            if (dfa.isEndNode(node))
                emitJESuccess();
            else
                emitJEFail();
            dfa.iterateEdges(node, [&](char c, DFANode destination) {
                machineCode.push_back(CMP_AL);
                machineCode.push_back(c);
                emitJE();
                nodeAddressLocations.emplace_back(std::make_pair(machineCode.size(), destination));
                emitPointerSpace();
            });
            emitJMPFail();
        });
        
        successLocation = machineCode.size();
        machineCode.push_back(MOV_AL);
        machineCode.push_back(1);
        emitJMPEpilogue();
        
        failLocation = machineCode.size();
        machineCode.push_back(MOV_AL);
        machineCode.push_back(0);
        
        epilogueLocation = machineCode.size();
        machineCode.push_back(POP_RDI);
        machineCode.push_back(POP_RBP);
        machineCode.push_back(RET);
    }
    
    void overwrite32WithDelta(size_t location, size_t destination) {
        *reinterpret_cast<uint32_t*>(machineCode.data() + location) = static_cast<uint32_t>(destination - (location + 4));
    }
    
    void linkMachineCode(const DFA& dfa) {
        for (const auto& nodeAddressLocation : nodeAddressLocations)
            overwrite32WithDelta(nodeAddressLocation.first, stateLocations[nodeAddressLocation.second]);
        
        for (size_t location : successAddressLocations)
            overwrite32WithDelta(location, successLocation);
        
        for (size_t location : failAddressLocations)
            overwrite32WithDelta(location, failLocation);
        
        for (size_t location : epilogueAddressLocations)
            overwrite32WithDelta(location, epilogueLocation);
    }
    
    void emitNextCharCall() {
        machineCode.push_back(REX_000);
        machineCode.push_back(MOV_MR);
        machineCode.push_back(0x7d); // immediate 8-bit offset from %ebp
        machineCode.push_back(-0x8); // "this" ptr
        
        machineCode.push_back(CALL);
        nextCharAddressLocations.insert(machineCode.size());
        emitPointerSpace();
    }
    
    void emitJESuccess() {
        emitJE();
        successAddressLocations.insert(machineCode.size());
        emitPointerSpace();
    }
    
    void emitJEFail() {
        emitJE();
        failAddressLocations.insert(machineCode.size());
        emitPointerSpace();
    }
    
    void emitJMPFail() {
        machineCode.push_back(JMP);
        failAddressLocations.insert(machineCode.size());
        emitPointerSpace();
    }
    
    void emitJMPEpilogue() {
        machineCode.push_back(JMP);
        epilogueAddressLocations.insert(machineCode.size());
        emitPointerSpace();
    }
    
    void emitJE() {
        machineCode.push_back(JNE32_1);
        machineCode.push_back(JNE32_2);
    }
    
    void emitPointerSpace() {
        for (int i(0); i < 4; ++i)
            machineCode.push_back(0);
    }
    
    std::vector<uint8_t> takeMachineCode() {
        return std::move(machineCode);
    }
    
    std::unordered_set<size_t> takeNextCharAddressLocations() {
        return std::move(nextCharAddressLocations);
    }
    
private:
    const uint8_t RET = 0xc3;
    const uint8_t CALL = 0xe8;
    const uint8_t CMP_AL = 0x3c;
    const uint8_t JMP = 0xe9;
    const uint8_t JNE32_1 = 0x0f;
    const uint8_t JNE32_2 = 0x84;
    const uint8_t MOV_AL = 0xb0;
    const uint8_t PUSH_RBP = 0x55;
    const uint8_t PUSH_RDI = 0x57;
    const uint8_t POP_RBP = 0x5d;
    const uint8_t REX_000 = 0x48;
    const uint8_t MOV_RR = 0x89;
    const uint8_t MOV_MR = 0x8b;
    const uint8_t POP_RDI = 0x5f;

    size_t successLocation;
    size_t failLocation;
    size_t epilogueLocation;
    std::vector<uint8_t> machineCode;
    std::unordered_set<size_t> nextCharAddressLocations;
    std::unordered_set<size_t> successAddressLocations;
    std::unordered_set<size_t> failAddressLocations;
    std::unordered_set<size_t> epilogueAddressLocations;
    std::vector<std::pair<size_t, DFANode>> nodeAddressLocations;
    std::vector<std::size_t> stateLocations;
};

class Tracker {
public:
    Tracker(const std::string& s, void* machineCode): index(0), s(s), machineCode(machineCode) {
    }
    
    bool run() {
        bool (Tracker::*callMe)()(nullptr);
        // AFAICT, we have to drop down to asm to cast a void* to a pointer to member function
        // Note that a pointer to a member function is actually a tuple.
        asm("movq %1, %0" : "=m" (callMe) : "r" (machineCode));
        return (this->*callMe)();
    }
    
private:
    friend class JIT;
    
    char nextChar() {
        if (index == s.length())
            return 0;
        return s[index++];
    }
    
    size_t index;
    std::string s;
    void* machineCode;
};
    
class Unmapper {
public:
    Unmapper(std::size_t size): size(size) {
    }

    void operator()(void* ptr) const {
        int ret(munmap(ptr, size));
        if (ret != 0)
            perror("Unmap failed");
        assert(ret == 0);
    }

private:
    std::size_t size;
};

JIT::JIT(const DFA& dfa) {
    CodeGenerator codeGenerator(dfa);
    std::vector<uint8_t> code(codeGenerator.takeMachineCode());
    machineCode = std::shared_ptr<void>(mmap(NULL, code.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0), Unmapper(code.size()));
    if (machineCode.get() == MAP_FAILED)
        perror("Map failed");
    assert(machineCode.get() != MAP_FAILED);
    memcpy(machineCode.get(), code.data(), code.size());

    for (size_t i : codeGenerator.takeNextCharAddressLocations()) {
        uint8_t* location(reinterpret_cast<uint8_t*>(machineCode.get()) + i);
        uint8_t* pc(location + 4);
        uint8_t* nextCharFunction;
        // AFAICT, we have to drop down to asm to cast a function pointer to a uint8_t*
        asm("movq %1, %0" : "=r" (nextCharFunction) : "r" (&Tracker::nextChar));
        *reinterpret_cast<uint32_t*>(location) = static_cast<uint32_t>(nextCharFunction - pc);
    }
}

bool JIT::operator()(const std::string& s) const {
    Tracker t(s, machineCode.get());
    return t.run();
}

}