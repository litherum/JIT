//
//  main.cpp
//  Regex
//
//  Created by Litherum on 7/11/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <assert.h>
#include <sys/mman.h>

#if !defined(__x86_64__)
#error Requires x86_64!
#endif

class Node {
public:
    void addEdge(char c, Node& n) {
        edges.insert(std::make_pair(c, std::reference_wrapper<Node>(n)));
    }

    Node* follow(char c) const {
        auto iter = edges.find(c);
        if (iter == edges.end())
            return nullptr;
        return &iter->second.get();
    }

    template<typename T>
    void iterateEdges(T callback) const {
        for (auto iter : edges)
            callback(iter.first, iter.second.get());
    }

private:
    std::map<char, std::reference_wrapper<Node>> edges;
};

class NodeComparator {
public:
    bool operator() (const std::reference_wrapper<Node> x, const std::reference_wrapper<Node> y) const {
        return &x.get() < &y.get();
    }
};

typedef std::set<std::reference_wrapper<Node>, NodeComparator> NodeCollection;

class Machine {
public:
    virtual ~Machine() {
    }

    virtual bool run(const std::string& s) const = 0;
};

class Interpreter: public Machine {
public:
    Interpreter(const NodeCollection&, const Node& startNode, const Node& endNode): startNode(startNode), endNode(endNode) {
    }

    virtual bool run(const std::string& s) const override {
        const Node* state = &startNode;
        for (char c : s) {
            if (Node* n = state->follow(c))
                state = n;
            else
                return false;
        }
        return state == &endNode;
    }

private:
    const Node& startNode;
    const Node& endNode;
};

class CodeGenerator {
public:
    CodeGenerator(const NodeCollection& nodes, const Node& startNode, const Node& endNode) {
        compileMachineCode(nodes, startNode, endNode);
        linkMachineCode(startNode, endNode);
    }
    
    void compileMachineCode(const NodeCollection& nodes, const Node& startNode, const Node& endNode) {
        machineCode.push_back(PUSH_RBP);

        machineCode.push_back(REX_000);
        machineCode.push_back(MOV_RR);
        machineCode.push_back(0xe5); // stack pointer into base pointer
        
        machineCode.push_back(PUSH_RDI); // "this" ptr

        machineCode.push_back(JMP);
        startAddressLocation = machineCode.size();
        emitPointerSpace();

        for (const Node& node : nodes) {
            stateLocations.insert(std::make_pair(&node, machineCode.size()));
            emitNextCharCall();
            machineCode.push_back(CMP_AL);
            machineCode.push_back(0);
            if (&node == &endNode)
                emitJESuccess();
            else
                emitJEFail();
            node.iterateEdges([&](char c, Node& d) {
                machineCode.push_back(CMP_AL);
                machineCode.push_back(c);
                emitJE();
                nodeAddressLocations.push_back(std::make_pair(machineCode.size(), std::reference_wrapper<Node>(d)));
                emitPointerSpace();
            });
            emitJMPFail();
        }

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

    void linkMachineCode(const Node& startNode, const Node& endNode) {
        assert(stateLocations.find(&startNode) != stateLocations.end());
        assert(stateLocations.find(&endNode) != stateLocations.end());

        overwrite32WithDelta(startAddressLocation, stateLocations[&startNode]);

        for (const auto& nodeAddressLocation : nodeAddressLocations) {
            auto iter = stateLocations.find(&nodeAddressLocation.second.get());
            assert(iter != stateLocations.end());
            overwrite32WithDelta(nodeAddressLocation.first, iter->second);
        }

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
        for (int i = 0; i < 4; ++i)
            machineCode.push_back(0);
    }

    std::vector<uint8_t> takeMachineCode() {
        return std::move(machineCode);
    }

    std::set<size_t> takeNextCharAddressLocations() {
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

    size_t startAddressLocation;
    size_t successLocation;
    size_t failLocation;
    size_t epilogueLocation;
    std::vector<uint8_t> machineCode;
    std::set<size_t> nextCharAddressLocations;
    std::set<size_t> successAddressLocations;
    std::set<size_t> failAddressLocations;
    std::set<size_t> epilogueAddressLocations;
    std::vector<std::pair<size_t, std::reference_wrapper<Node>>> nodeAddressLocations;
    std::map<const Node*, size_t> stateLocations;
};

class JIT: public Machine {
public:
    JIT(const NodeCollection& nodes, const Node& startNode, const Node& endNode) {
        CodeGenerator codeGenerator(nodes, startNode, endNode);
        std::vector<uint8_t> code = codeGenerator.takeMachineCode();
        machineCode.reset(mmap(NULL, code.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0));
        if (machineCode.get() == MAP_FAILED)
            perror("Map failed");
        assert(machineCode.get() != MAP_FAILED);
        memcpy(machineCode.get(), code.data(), code.size());
        machineCode.get_deleter().setLength(code.size());

        for (size_t i : codeGenerator.takeNextCharAddressLocations()) {
            uint8_t* location = reinterpret_cast<uint8_t*>(machineCode.get()) + i;
            uint8_t* pc = location + 4;
            uint8_t* nextCharFunction;
            // AFAICT, we have to drop down to asm to cast a function pointer to a uint8_t*
            asm("movq %1, %0" : "=r" (nextCharFunction) : "r" (&Tracker::nextChar));
            *reinterpret_cast<uint32_t*>(location) = static_cast<uint32_t>(nextCharFunction - pc);
        }
    }
    
    virtual bool run(const std::string& s) const override {
        Tracker t(s, machineCode.get());
        return t.run();
    }

private:
    class Tracker {
    public:
        Tracker(const std::string& s, void* machineCode): index(0), s(s), machineCode(machineCode) {
        }
        
        bool run() {
            bool (JIT::Tracker::*callMe)() = nullptr;
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
        void operator()(void* ptr) const {
            int ret = munmap(ptr, length);
            if (ret != 0)
                perror("Unmap failed");
            assert(ret == 0);
        }

        void setLength(size_t length) {
            this->length = length;
        }

    private:
        size_t length;
    };

    std::unique_ptr<void, Unmapper> machineCode;
};

std::unique_ptr<Machine> compile(const NodeCollection& nodes, const Node& startNode, const Node& endNode) {
    return std::unique_ptr<JIT>(new JIT(nodes, startNode, endNode));
}

int main(int argc, const char * argv[]) {
    Node startNode;
    Node endNode;
    NodeCollection nodes;
    nodes.insert(startNode);
    nodes.insert(endNode);
    startNode.addEdge('1', startNode);
    startNode.addEdge('0', endNode);

    std::unique_ptr<Machine> m = compile(nodes, startNode, endNode);
    std::cout << m->run("110") << std::endl;
    std::cout << m->run("0") << std::endl;
    std::cout << m->run("1010") << std::endl;
    std::cout << m->run("11") << std::endl;
    std::cout << m->run("112") << std::endl;
    return 0;
}
