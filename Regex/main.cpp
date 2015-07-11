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
#include <string>
#include <vector>
#include <assert.h>
#include <sys/mman.h>

class Node {
public:
    void addEdge(char c, Node& n) {
        edges.insert(std::make_pair(c, std::reference_wrapper<Node>(n)));
    }
    Node* follow(char c) {
        auto iter = edges.find(c);
        if (iter == edges.end())
            return nullptr;
        return &iter->second.get();
    }
private:
    std::map<char, std::reference_wrapper<Node>> edges;
};

class Machine {
public:
    virtual bool run(std::string s) const = 0;
};

class Interpreter: public Machine {
public:
    Interpreter(Node& startNode, Node& endNode): startNode(startNode), endNode(endNode) {
    }

    virtual bool run(std::string s) const override {
        Node* state = &startNode;
        for (char c : s) {
            if (Node* n = state->follow(c))
                state = n;
            else
                return false;
        }
        return state == &endNode;
    }

private:
    Node& startNode;
    Node& endNode;
};

static std::vector<uint8_t> emitMachineCode(Node& startNode, Node& endNode) {
    std::vector<uint8_t> machineCode;
    machineCode.push_back(0xc3); // ret
    return machineCode;
}

class JIT: public Machine {
public:
    JIT(Node& startNode, Node& endNode) {
        std::vector<uint8_t> data(emitMachineCode(startNode, endNode));
        machineCode.reset(mmap(NULL, data.size(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0));
        if (machineCode.get() == MAP_FAILED)
            perror("Map failed");
        assert(machineCode.get() != MAP_FAILED);
        memcpy(machineCode.get(), data.data(), data.size());
        machineCode.get_deleter().setLength(data.size());
    }
    
    virtual bool run(std::string s) const override {
        Tracker t(s, machineCode.get());
        return t.run();
    }

private:
    class Tracker {
    public:
        Tracker(std::string s, void* machineCode): index(0), s(s), machineCode(machineCode) {
        }

        bool run();

    private:
        char nextChar();

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

bool JIT::Tracker::run() {
    reinterpret_cast<void(*)()>(machineCode)();
    char c;
state1:
    c = nextChar();
    if (c == 0)
        goto fail;
    else if (c == '1')
        goto state1;
    else if (c == '0')
        goto state2;
    else
        goto fail;

state2:
    c = nextChar();
    if (c == 0)
        goto succeed;
    else
        goto fail;

succeed:
    return true;
fail:
    return false;
}

char JIT::Tracker::nextChar() {
    if (index == s.length())
        return 0;
    return s[index++];
}

std::unique_ptr<Machine> compile(Node& startNode, Node& endNode) {
    return std::unique_ptr<JIT>(new JIT(startNode, endNode));
}

int main(int argc, const char * argv[]) {
    Node startNode;
    Node endNode;
    startNode.addEdge('1', startNode);
    startNode.addEdge('0', endNode);
    std::unique_ptr<Machine> m = compile(startNode, endNode);
    std::cout << m->run("111110") << std::endl;
    return 0;
}
