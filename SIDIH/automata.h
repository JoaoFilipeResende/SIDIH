#pragma once
#include "stdafx.h"
#include <map>
#include <vector>
#include <string>


class Automato {
private:
	std::vector<std::string> states;
	std::vector<std::string> events;
	/*
	*	           |........KEY.......| |.....ELEMENT....|		
	*	transitions[{estado_A, evento}].pushback(estado_B);
	*/
	std::map<std::pair<int, int>, std::vector<int>> transitions;
	std::map<std::pair<int, int>, std::vector<int>> DFA_transitions;
	int initial;
	std::vector<int> final;

	void getAccessibleStates(int STATE, std::vector<int>& VECT);
	void getCoAccessibleStates(int STATE, std::vector<int>& VECT);
	bool cleanAutomatStates(std::vector<int>& VECT); 
	void convertNFA(std::vector<int> state_anterior, int event, std::vector<int> state, std::vector<std::vector<int>>& VECT, int flag);
	void minimization(std::vector<std::vector<int>>& vect, std::vector<std::vector<int>>& nonvect);
	void reuniao(std::vector<int> inicial,int& prox, std::vector<std::vector<int>>& vect, int& flag);
	std::stringstream stringAutomato(std::vector<std::vector<int>> state, std::vector<int> event, int inicial, std::vector<int> state_final, std::map<std::pair<int, int>, std::vector<int>> mapa);
	std::stringstream stringAutomatoForDFA(std::vector<std::vector<int>> state, std::vector<int> event, int inicial, std::vector<int> state_final, std::map<std::pair<int, int>, std::vector<int>> mapa);
	void productDFA(int chosen_automato_A, int chosen_automato_B, std::vector<Automato> automatos);
	void parallelDFA(int chosen_automato_A, int chosen_automato_B, std::vector<Automato> automatos);
public:
	std::string automato_Path;

	bool loadFile(const char* file_path);	/*Carrega o fichiero para uma variável*/
	bool parseAutomato(std::istream& myfile);	/*mudar esta função para bool, caso seja lido um estado tipo S5 (nao existe) devia dar erro*/
	void printAutomato(std::ostream& console_output);  /*Imprime o Autómato com a informação recolhida do ficheiro introduzido.*/
	bool checkDFANFA(int ask);	/*Verificar se o autómato é deterministico ou não deterministico*/
	bool checkAccessibility(int ask);
	bool checkCoAccessibility(int ask);
	void clearAutomat(void);
	void convertNFAtoDFA(void);
	bool minimizationDFA(void);
	void productDFAteste(int chosen_automato_A, int chosen_automato_B, std::vector<Automato>& automatos);
	void parallelDFAteste(int chosen_automato_A, int chosen_automato_B, std::vector<Automato>& automatos);
	bool saveToFile(std::string Path);
};

