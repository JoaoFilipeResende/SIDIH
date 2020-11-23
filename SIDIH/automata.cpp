#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <algorithm>
#include "automata.h"

bool Automato::loadFile(const char* file_path)
{
	std::ifstream myfile(file_path);

	if (myfile.is_open())
	{
		clearAutomat();
		if (parseAutomato(myfile))
		{
			myfile.close();
			return true;
		}
		else
		{
			std::cout << "Error on function ->Parse_Aut<-" << std::endl;
			return false;
		}
	}

	std::cout << "Unable to open file, Check file location!" << std::endl;
	return false;
}

void Automato::printAutomato(std::ostream& console_output) //FALTA IMPRMIR O f - transições 
{
	system("cls");

	console_output << "Automato: " << automato_Path << std::endl;
	console_output << "\nG=(X,E,f,Xinit,Xfinal)\n" << std::endl;

	console_output << "X={";
	for (int i = 0; i != states.size(); i++)
	{
		console_output << states[i];
		if (i < states.size() - 1)
		{
			console_output << ",";
		}
	}
	console_output << "}" << std::endl;

	console_output << "E={";
	for (int i = 0; i != events.size(); i++)
	{
		console_output << events[i];
		if (i < events.size() - 1)
		{
			console_output << ",";
		}
	}
	console_output << "}" << std::endl;

	console_output << "Xinit=" << states[initial] << std::endl;

	console_output << "Xfinal={";
	for (int i = 0; i != final.size(); i++)
	{
		console_output << states[final[i]];
		if (i < final.size() - 1)
		{
			console_output << ",";
		}
	}
	console_output << "}" << std::endl;

	console_output << "TRANSITIONS:" << std::endl;
	for(auto &entry: transitions) 
	{
		console_output << "f(" << states.at(entry.first.first) << "," << events.at(entry.first.second) << ")={";
		for (auto j = 0; j != entry.second.size(); j++)
		{
			console_output << states.at(entry.second.at(j));
			if (j < entry.second.size()-1)
			{
				console_output << ",";
			}
		}
		console_output << "}" << std::endl;
	}
	console_output << "\n";
}

bool Automato::saveToFile(std::string Path)
{
	std::stringstream stringautomato;

	stringautomato << "STATES" << "\n";
	for (auto i = 0; i != states.size(); i++)
	{
		stringautomato << states.at(i) << "\n";
	}

	stringautomato << "EVENTS" << "\n";
	for (auto i = 0; i != events.size(); i++)
	{
		stringautomato << events.at(i) << "\n";
	}

	stringautomato << "TRANSITIONS" << "\n";
	for (auto &entry : transitions)
	{
		stringautomato << states.at(entry.first.first);

		stringautomato << " " << events.at(entry.first.second) << " ";

		for (auto j = 0; j != entry.second.size(); j++)
		{
			stringautomato << states.at(entry.second.at(j));
		}
		stringautomato << "" << std::endl;
	}

	stringautomato << "INITIAL" << "\n";

	stringautomato << states.at(initial) << "\n";

	stringautomato << "FINAL" << "\n";
	for (auto n = 0; n != final.size(); n++)
	{
		if (find(states.begin(), states.end(), states.at(final.at(n))) != states.end())
		{
			stringautomato << states.at(final.at(n));
			stringautomato << "\n";
		}
	}

	stringautomato << "END" << "\n";

	std::ofstream datFile;
	datFile.open(Path, std::ofstream::binary);
	datFile.write(stringautomato.str().c_str(), stringautomato.str().length());
	datFile.close();
	
	return true;
}

void Automato::clearAutomat(void)
{
	states = std::vector<std::string>();
	events = std::vector<std::string>();
	transitions.clear();
	initial = int();
	final = std::vector<int>();
}

bool Automato::parseAutomato(std::istream& myfile) /* VERIFICAR SE É ACESSIVEL! VERIFICAR SE TEM UM ESTADO QUE NAO SE ENCONTRA NO VETOR DE ESTADOS E VER MAIS ERROS */
{
	clearAutomat();

	std::string line;
	int number_of_chars = 0;
	int number_of_lines = 0;
	std::vector<std::string> info;

	while (std::getline(myfile, line))
	{
		number_of_lines++;
		number_of_chars += line.length();
		info.push_back(line);
	}

	size_t pos_STATES, pos_EVENTS, pos_TRANSITIONS, pos_INITIAL, pos_FINAL, pos_END;

	if (find(info.begin(), info.end(), "STATES") == info.end())
	{
		std::cout << "Error: Line STATES not found!" << std::endl;
		return false;
	}
	if (find(info.begin(), info.end(), "EVENTS") == info.end())
	{
		std::cout << "Error: Line EVENTS not found!" << std::endl;
		return false;
	}
	if (find(info.begin(), info.end(), "TRANSITIONS") == info.end())
	{
		std::cout << "Error: Line TRANSITIONS not found!" << std::endl;
		return false;
	}
	if (find(info.begin(), info.end(), "INITIAL") == info.end())
	{
		std::cout << "Error: Line INITIAL not found!" << std::endl;
		return false;
	}
	if (find(info.begin(), info.end(), "FINAL") == info.end())
	{
		std::cout << "Error: Line FINAL not found!" << std::endl;
		return false;
	}
	if (find(info.begin(), info.end(), "END") == info.end())
	{
		std::cout << "Error: Line END not found!" << std::endl;
		return false;
	}

	pos_STATES = (find(info.begin(), info.end(), "STATES") - info.begin());
	pos_EVENTS = (find(info.begin(), info.end(), "EVENTS") - info.begin());
	pos_TRANSITIONS = (find(info.begin(), info.end(), "TRANSITIONS") - info.begin());
	pos_INITIAL = (find(info.begin(), info.end(), "INITIAL") - info.begin());
	pos_FINAL = (find(info.begin(), info.end(), "FINAL") - info.begin());
	pos_END = (find(info.begin(), info.end(), "END") - info.begin());

	/* Read Data between STATES and EVENTS*/

	while (pos_STATES < (pos_EVENTS - 1)) /* (pos_EVENTS - 1) para obter a posição do ultimo estado*/
	{
		pos_STATES++;
		info.at(pos_STATES).erase(std::remove(info.at(pos_STATES).begin(), info.at(pos_STATES).end(), ' '), info.at(pos_STATES).end());
		states.push_back(info.at(pos_STATES));
	}
	while (pos_EVENTS < (pos_TRANSITIONS - 1))
	{
		pos_EVENTS++;
		info.at(pos_EVENTS).erase(std::remove(info.at(pos_EVENTS).begin(), info.at(pos_EVENTS).end(), ' '), info.at(pos_EVENTS).end());
		events.push_back(info.at(pos_EVENTS));
	}

	/*
	*	           |........KEY.......| |.....ELEMENT....|
	*	transitions[{estado_A, evento}].pushback(estado_B);
	*/

	std::string delimiter = " ";
	std::vector<std::string> info_mapa;
	int estado_A, evento, estado_B;

	while (pos_TRANSITIONS < (pos_INITIAL - 1))
	{
		info_mapa = std::vector<std::string>();

		pos_TRANSITIONS++;

		/* Leitura das transições */

		size_t pos = info.at(pos_TRANSITIONS).find(delimiter);
		while (pos != std::string::npos)
		{
			info_mapa.push_back(info.at(pos_TRANSITIONS).substr(0, pos));
			info.at(pos_TRANSITIONS).erase(0, pos + delimiter.length());
			pos = info.at(pos_TRANSITIONS).find(delimiter);
		}
		info_mapa.push_back(info.at(pos_TRANSITIONS));

		if (info_mapa.size() > 3)
		{
			std::cout << "Error: Invalid transition format (line " << pos_TRANSITIONS << ")" << std::endl;
			return false;
		}

		estado_A = (find(states.begin(), states.end(), info_mapa.at(0)) - states.begin());
		evento = (find(events.begin(), events.end(), info_mapa.at(1)) - events.begin());
		estado_B = (find(states.begin(), states.end(), info_mapa.at(2)) - states.begin());

		transitions[{estado_A, evento}].push_back(estado_B);
	}

	if ( (pos_FINAL - pos_INITIAL) > 1)
	{
		if (((pos_FINAL - 1) - pos_INITIAL) == 1)
		{
			while (pos_INITIAL < (pos_FINAL - 1))
			{
				pos_INITIAL++;
				info.at(pos_INITIAL).erase(std::remove(info.at(pos_INITIAL).begin(), info.at(pos_INITIAL).end(), ' '), info.at(pos_INITIAL).end());
				if (find(states.begin(), states.end(), info.at(pos_INITIAL)) == states.end())
				{
					std::cout << "Error: Inicial state is not part of this automato! (line " << pos_INITIAL + 1 << ")" << std::endl;
					return false;
				}
				initial = (find(states.begin(), states.end(), info.at(pos_INITIAL)) - states.begin());
			}
		}
		else
		{
			std::cout << "Error: Only one Inicial state! (line " << pos_INITIAL + 1 << ")" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "Error: No Inicial state! (line " << pos_INITIAL + 1 << ")" << std::endl;
		return false;
	}

	if ((pos_END - pos_FINAL) > 1)
	{
		while (pos_FINAL < (pos_END - 1))
		{
			pos_FINAL++;
			info.at(pos_FINAL).erase(std::remove(info.at(pos_FINAL).begin(), info.at(pos_FINAL).end(), ' '), info.at(pos_FINAL).end());
			if (find(states.begin(), states.end(), info.at(pos_FINAL)) == states.end())
			{
				std::cout << "Error: Final state is not part of this automato! (line " << pos_INITIAL + 1 << ")" << std::endl;
				return false;
			}
			final.push_back((find(states.begin(), states.end(), info.at(pos_FINAL)) - states.begin()));
		}
	}
	else
	{
		std::cout << "Error: No Final state! (line " << pos_FINAL + 1 << ")" << std::endl;
		return false;
	}
	
	if (pos_END == ( number_of_lines -1 ))
	{
		printAutomato(std::cout);
		return true;
	}
	else
	{
		return false;
	}
}

std::stringstream Automato::stringAutomato(std::vector<std::vector<int>> state, std::vector<int> event, int inicial, std::vector<int> state_final, std::map<std::pair<int, int>, std::vector<int>> mapa)
{
	std::stringstream stringautomato;

	stringautomato << "STATES" << "\n";
	for (auto i = 0; i != state.size(); i++)
	{
		for (auto j = 0; j != state.at(i).size(); j++)
		{
			stringautomato << states.at(state.at(i).at(j));
		}
		stringautomato << "\n";
	}

	stringautomato << "EVENTS" << "\n";
	for (auto i = 0; i != event.size(); i++)
	{
		stringautomato << events.at(event.at(i)) << "\n";
	}

	stringautomato << "TRANSITIONS" << "\n";

	for (auto &entry : mapa) 
	{
		for (auto j = 0; j != state.at(entry.first.first).size(); j++)
		{
			stringautomato << states.at(state.at(entry.first.first).at(j));
		}

		stringautomato << " " << events.at(entry.first.second) << " ";

		for (auto j = 0; j != entry.second.size(); j++)
		{
			for (auto f = 0; f != state.at(entry.second.at(j)).size(); f++)
			{
				stringautomato << states.at(state.at(entry.second.at(j)).at(f));
			}
		}
		stringautomato << "" << std::endl;
	}

	stringautomato << "INITIAL" << "\n"; 

	for (auto i = 0; i < state.at(inicial).size(); i++)
	{
		stringautomato << states.at(state.at(inicial).at(i));
	}

	stringautomato << "\n";

	stringautomato << "FINAL" << "\n";

	for (auto n = 0; n != state_final.size(); n++)
	{
		if (find(state.begin(), state.end(), state.at(state_final.at(n))) != state.end())
		{
			for (auto g = 0; g != state.at(state_final.at(n)).size(); g++)
			{
				stringautomato << states.at(state.at(state_final.at(n)).at(g));
			}

			stringautomato << "\n";
		}
	}

	stringautomato << "END" << "\n";

	return stringautomato;
}

std::stringstream Automato::stringAutomatoForDFA(std::vector<std::vector<int>> state, std::vector<int> event, int inicial, std::vector<int> state_final, std::map<std::pair<int, int>, std::vector<int>> mapa)
{
	std::stringstream stringautomato;

	stringautomato << "STATES" << "\n";
	for (auto i = 0; i != state.size(); i++)
	{
		for (auto j = 0; j != state.at(i).size(); j++)
		{
			if (state.at(i).size() > 1 && j == 0)
			{
				stringautomato << "(";
			}

			stringautomato << states.at(state.at(i).at(j));

			if (j != state.at(i).size() - 1)
			{
				stringautomato << ",";
			}
			
			if (state.at(i).size() > 1 && j == state.at(i).size() - 1)
			{
				stringautomato << ")";
			}
		}
		stringautomato << "\n";
	}

	stringautomato << "EVENTS" << "\n";
	for (auto i = 0; i != event.size(); i++)
	{
		stringautomato << events.at(event.at(i)) << "\n";
	}

	stringautomato << "TRANSITIONS" << "\n";

	for (auto &entry : mapa)
	{
		for (auto j = 0; j != state.at(entry.first.first).size(); j++)
		{
			if (state.at(entry.first.first).size() > 1 && j == 0)
			{
				stringautomato << "(";
			}

			stringautomato << states.at(state.at(entry.first.first).at(j));

			if (j != state.at(entry.first.first).size() - 1)
			{
				stringautomato << ",";
			}
			
			if (state.at(entry.first.first).size() > 1 && j == state.at(entry.first.first).size() - 1)
			{
				stringautomato << ")";
			}
		}
		
		stringautomato << " " << events.at(entry.first.second) << " ";

		for (auto j = 0; j != entry.second.size(); j++)
		{
			for (auto f = 0; f != state.at(entry.second.at(j)).size(); f++)
			{
				if (state.at(entry.second.at(j)).size() > 1 && f == 0)
				{
					stringautomato << "(";
				}

				stringautomato << states.at(state.at(entry.second.at(j)).at(f));

				if (f != state.at(entry.second.at(j)).size() - 1)
				{
					stringautomato << ",";
				}
				
				if (state.at(entry.second.at(j)).size() > 1 && f == state.at(entry.second.at(j)).size() - 1)
				{
					stringautomato << ")";
				}
			}
		}
		stringautomato << "" << std::endl;
	}

	stringautomato << "INITIAL" << "\n";

	for (auto i = 0; i < state.at(inicial).size(); i++)
	{
		if (state.at(inicial).size() > 1 && i == 0)
		{
			stringautomato << "(";
		}

		stringautomato << states.at(state.at(inicial).at(i));

		if (i != state.at(inicial).size() - 1)
		{
			stringautomato << ",";
		}
		
		if (state.at(inicial).size() > 1 && i == state.at(inicial).size() - 1)
		{
			stringautomato << ")";
		}
	}

	stringautomato << "\n";

	stringautomato << "FINAL" << "\n";

	for (auto n = 0; n != state_final.size(); n++)
	{
		if (find(state.begin(), state.end(), state.at(state_final.at(n))) != state.end())
		{
			for (auto g = 0; g != state.at(state_final.at(n)).size(); g++)
			{
				if (state.at(state_final.at(n)).size() > 1 && g == 0)
				{
					stringautomato << "(";
				}

				stringautomato << states.at(state.at(state_final.at(n)).at(g));

				if (g != state.at(state_final.at(n)).size() - 1)
				{
					stringautomato << ",";
				}

				if (state.at(state_final.at(n)).size() > 1 && g == state.at(state_final.at(n)).size() - 1)
				{
					stringautomato << ")";
				}
			}
			stringautomato << "\n";
		}
	}

	stringautomato << "END" << "\n";

	return stringautomato;
}

bool Automato::checkDFANFA(int ask)
{
	int DFA = 1;
	for (auto i = 0; i != events.size(); i++)
	{
		for (auto j = 0; j != states.size(); j++)
		{
			if (transitions.count({ j,i }) == 1)
			{
				if (transitions.at({ j,i }).size() > 1)
				{			
					DFA = 0;
					break;
				}
			}
		}
	}

	if (DFA == 1)
	{
		std::cout << "This is a deterministic finite automaton (DFA)" << std::endl;
		return true;
	}
	else if (DFA == 0 && ask == 1)
	{
		std::string temp;
		std::cout << "This is a nondeterministic finite automaton (NFA)" << std::endl;
		std::cout << "Convert this automato to DFA? (y/n)" << std::endl;
		std::cin >> temp;
		getchar();
		if (temp == "yes" || temp == "y" || temp == "YES" || temp == "Y")
		{
			convertNFAtoDFA();
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ask == 0)
	{
		std::cout << "This is a nondeterministic finite automaton (NFA)" << std::endl;
		std::cout << "Converting this automato to DFA..." << std::endl;
		convertNFAtoDFA();
		return true;
	}
}

void Automato::getAccessibleStates(int STATE, std::vector<int>& VECT) //string stream 
{
	VECT.push_back(STATE); 

	for (auto j = 0; j != events.size(); j++)
	{
		if (transitions.count({ STATE,j }) == 1)
		{
			for (auto i = 0; i != transitions.at({ STATE,j }).size(); i++)
			{
				if (find(VECT.begin(), VECT.end(), transitions.at({ STATE,j }).at(i)) == VECT.end())
				{
					getAccessibleStates(transitions.at({ STATE,j }).at(i), VECT);
				}
			}
		}
	}
}

bool Automato::checkAccessibility(int ask)
{
	std::vector<int> statesacess;
	statesacess = std::vector<int> ();

	getAccessibleStates(initial, statesacess);

	/*Check if there is at least one final state!*/

	for (auto i = 0; i != final.size(); i++)
	{
		if (find(statesacess.begin(), statesacess.end(), final.at(i)) != statesacess.end())
		{
			break;
		}
		else
		{
			if (i == (final.size() - 1))
			{
				std::cout << "Error: No Final State is accessible!" << std::endl;
				return false;
			}
		}
	}

	if (statesacess.size() == states.size())
	{
		std::cout << "This automaton is acessible" << std::endl;
		return true;
	}
	else if (ask == 1)
	{
		std::string temp;
		std::cout << "This automaton is not acessible" << std::endl;
		std::cout << "Convert this automato to acessible? (y/n)" << std::endl;
		std::cin >> temp;
		getchar();
		if (temp == "yes" || temp == "y" || temp == "YES" || temp == "Y" )
		{
			cleanAutomatStates(statesacess);	
			return true;
		}
		else 
		{
			return false;
		}
	}
	else if(ask == 0)
	{
		std::cout << "This automaton is not Acessible" << std::endl;
		std::cout << "Converting this automato to Acessible..." << std::endl;
		cleanAutomatStates(statesacess);
		return true;
	}
}

bool Automato::cleanAutomatStates(std::vector<int>& VECT)
{
	std::stringstream stringautomato;

	stringautomato << "STATES" << "\n";
	for (auto i = 0; i != VECT.size(); i++)
	{
		if (find(states.begin(), states.end(), states.at(VECT.at(i))) != states.end())
		{
			stringautomato << states.at(VECT.at(i)) << "\n";
		}
	}

	stringautomato << "EVENTS" << "\n";

	/* 
	*Read the transitions map and fill the events vector with the events in map,
	*some of the events may have been deleted because of the convergin to coaccessibility
	*/
	std::vector<int> events_temp;

	for (auto &entry : transitions) 
	{
		for (auto i = 0; i != entry.second.size(); i++)
		{
			if (find(VECT.begin(), VECT.end(), transitions.at({entry.first.first,entry.first.second}).at(i)) != VECT.end())
			{
				if (find(VECT.begin(), VECT.end(), entry.first.first) != VECT.end())
				{
					if (find(events_temp.begin(), events_temp.end(), entry.first.second) == events_temp.end())
					{
						events_temp.push_back(entry.first.second);
					}
				}
			}
		}
	}

	std::sort(events_temp.begin(), events_temp.end()); //Por os eventos por ordem
	
	for (auto i = 0; i != events_temp.size(); i++)
	{
		stringautomato << events.at(events_temp.at(i)) << "\n";
	}

	stringautomato << "TRANSITIONS" << "\n";
	for (int i = 0; i != VECT.size(); i++) {
		//Go through all events and check if a transition exists
		for (int k = 0; k != events.size(); k++) {
			if (transitions.count({ VECT.at(i), k}) == 1)
			{
				for (auto j = 0; j != transitions.at({ VECT.at(i),k }).size(); j++)
				{
					if (find(VECT.begin(), VECT.end(), transitions.at({ VECT.at(i),k }).at(j)) != VECT.end())
					{
						stringautomato << states.at(VECT.at(i)) << " " << events.at(k) << " " << states.at(transitions.at({ VECT.at(i),k }).at(j)) << "\n";
					}
				}
			} 
		}
	}
	stringautomato << "INITIAL" << "\n";
	stringautomato << states.at(initial) << "\n";

	stringautomato << "FINAL" << "\n";
	
	for (auto j = 0; j != final.size(); j++)
	{
		if (find(VECT.begin(), VECT.end(), final.at(j)) != VECT.end())
		{
			stringautomato << states.at(final.at(j)) << "\n";
		}
	}

	stringautomato << "END";
	
	parseAutomato(stringautomato);
	
	return true;
} 

void Automato::getCoAccessibleStates(int STATE, std::vector<int>& VECT)
{
	if (find(VECT.begin(), VECT.end(), STATE) == VECT.end())
	{
		VECT.push_back(STATE);
	}

	for (auto &entry : transitions)
	{
		for (auto j = 0; j != entry.second.size(); j++)
		{
			if (entry.second.at(j) == STATE)
			{
				if (find(VECT.begin(), VECT.end(), entry.first.first) == VECT.end())
				{
					getCoAccessibleStates(entry.first.first, VECT);
				}
			}
		}
	}
}

bool Automato::checkCoAccessibility(int ask)
{
	std::vector<int> statescoacess;
	statescoacess = std::vector<int>();

	for(auto j=0; j!=final.size();j++)
	getCoAccessibleStates(final.at(j), statescoacess);
	std::sort(statescoacess.begin(), statescoacess.end());

	if (statescoacess.size() == states.size())
	{
		std::cout << "This automaton is Coacessible" << std::endl;
		return true;
	}
	else if (ask == 1)
	{
		std::string temp;
		std::cout << "This automaton is not Coacessible" << std::endl;
		std::cout << "Convert this automato to Coacessible? (y/n)" << std::endl;
		std::cin >> temp;
		getchar();
		if (temp == "yes" || temp == "y" || temp == "YES" || temp == "Y")
		{
			cleanAutomatStates(statescoacess);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ask == 0)
	{
		std::cout << "This automaton is not Coacessible" << std::endl;
		std::cout << "Converting this automato to Coacessible..." << std::endl;
		cleanAutomatStates(statescoacess);
		return true;
	}
}

void Automato::convertNFA(std::vector<int> state_anterior, int event, std::vector<int> state, std::vector<std::vector<int>>& VECT, int flag)
{
	if (find(VECT.begin(), VECT.end(), state) == VECT.end())
	{
		VECT.push_back(state);
	}

	if (flag == 1)
	{
		for (auto j = 0; j != VECT.size(); j++)
		{
			if (state_anterior == VECT.at(j))
			{
				for (auto l = 0; l != VECT.size(); l++)
				{
					if (state == VECT.at(l))
					{
						DFA_transitions[{j, event}].push_back(l);
					}
				}
			}
		}
	}

	for (auto i = 0; i != state.size(); i++)
	{
		for (auto j = 0; j != events.size(); j++)
		{
			if (transitions.count({ state.at(i),j }) == 1)
			{
				std::vector<int> state_temp = transitions.at({ state.at(i),j });
				std::sort(state_temp.begin(), state_temp.end());
				if (find(VECT.begin(), VECT.end(), state_temp) == VECT.end())
				{
					convertNFA(state, j, state_temp, VECT, 1);
				}
				else
				{
					for (auto p = 0; p != VECT.size(); p++)
					{
						if (state == VECT.at(p))
						{
							for (auto l = 0; l != VECT.size(); l++)
							{
								std::vector<int> state_temp = transitions.at({ state.at(i),j });
								std::sort(state_temp.begin(), state_temp.end());
								
								if (state_temp  == VECT.at(l))
								{
									if (DFA_transitions.count({p,j})!=1)
									{
										DFA_transitions[{p, j}].push_back(l);
									}
									else
									{
										if (VECT.at(DFA_transitions.at({ p,j }).at(0)) != transitions.at({ state.at(i),j }))
										{
											std::vector<int> temp = VECT.at(DFA_transitions.at({ p,j }).at(0));
											std::sort(temp.begin(), temp.end());
											for (auto y = 0; y != transitions.at({ state.at(i),j }).size(); y++)
											{
												if (find(temp.begin(), temp.end(), transitions.at({ state.at(i),j }).at(y)) == temp.end())
												{
													temp.push_back(transitions.at({ state.at(i),j }).at(y));
													std::sort(temp.begin(), temp.end());
												}
												if(y == transitions.at({ state.at(i),j }).size()-1)
												{
													DFA_transitions.erase({ p, j });
													std::sort(temp.begin(), temp.end());
													if (find(VECT.begin(), VECT.end(), temp) != VECT.end())
													{
														for (auto s = 0; s != VECT.size(); s++)
														{
															if (temp == VECT.at(s))
															{
																DFA_transitions[{p, j}].push_back(s);
															}
														}
													}
													else
													{
														convertNFA(state, j, temp, VECT, 1);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Automato::convertNFAtoDFA(void)
{
	std::vector<std::vector<int>> DFA_STATES;
	
	int flag = 0;
	convertNFA({ initial }, 0, { initial }, DFA_STATES, flag);
	
	/* Read the transitions map and fill the events vector with the events in map */

	std::vector<int> events_temp;

	for (auto &entry : DFA_transitions) 
	{
		if (find(events_temp.begin(), events_temp.end(), entry.first.second) == events_temp.end())
		{
			events_temp.push_back(entry.first.second);
		}
	}
	std::sort(events_temp.begin(), events_temp.end()); /* Organizar por ordem os eventos ex: (b, a, c) -> (a, b, c) */

	/* Create the vector that contains the final states */

	std::vector<int> final_temp;
	for (auto i = 0; i != DFA_STATES.size(); i++)
	{
		for (auto n = 0; n != final.size(); n++)
		{
			if (find(DFA_STATES.at(i).begin(), DFA_STATES.at(i).end(), final.at(n)) != DFA_STATES.at(i).end())
			{
				final_temp.push_back(i);
			}
		}
	}
	
	/* Criação de um "ficheiro" na memoria */
	std::stringstream stringautomato;
	stringautomato = stringAutomato(DFA_STATES, events_temp, initial, final_temp, DFA_transitions);

	/* Atualização das variaveis do automato com a nova informação */
	parseAutomato(stringautomato);
}

void Automato::reuniao(std::vector<int> inicial,int& prox, std::vector<std::vector<int>>& vect, int& flag)
{
	for (auto i = prox; i < vect.size(); i++)
	{
		if (inicial != vect.at(i))
		{
			for (auto j = 0; j < vect.at(i).size(); j++)
			{
				if (find(inicial.begin(), inicial.end(), vect.at(i).at(j)) != inicial.end()) // existe reuniao possível
				{
					for (auto f = 0; f < vect.at(i).size(); f++)
					{
						if (prox <= vect.size() - 1)
						{
							if (find(inicial.begin(), inicial.end(), vect.at(i).at(f)) == inicial.end()) // existe reuniao possível
							{
								std::vector<std::vector<int>> temp = vect;
								vect.at(prox - 1).push_back(vect.at(i).at(f));
								vect.erase(vect.begin() + i);
								inicial = vect.at(prox - 1);
								flag = 1;
								i--;
								reuniao(inicial, prox, vect, flag);
							}
							else
							{
								if (f == vect.at(i).size() - 1) // nenhuma reuniao nova para realizar por isso apagamos a posicao do vetor
								{
									vect.erase(vect.begin() + i);
									flag = 1;
									i--;
									reuniao(inicial, prox, vect, flag);
								}
							}
						}
					}
				}			
			}
		}
		if (i == vect.size() - 1 && flag==0)
		{
			inicial = vect.at(prox);
			prox++;
			reuniao(inicial, prox , vect, flag);
		}
	}
}

bool Automato::minimizationDFA(void)
{
	if (checkDFANFA(1) == false)
	{
		std::cout << "Automato must be DFA" << std::endl;
		return false;
	}

	if (checkAccessibility(1) == false)
	{
		std::cout << "Automato must be Accessible." << std::endl;
		return false;
	}

	if (checkCoAccessibility(1) == false)
	{
		std::cout << "Automato must be CoAccessible." << std::endl;
		return false;
	}

	/* Applying the Myhill-Nerode theorem*/

	/*
	*Step 1 − Fill the vector <marked_state_pairs> with the marked states in order to get a states(Qi, Qj).
	*Consider every state pair(Qi, Qj) in the DFA where Qi ∈ F and Qj ∉ F or vice versa and mark them.[Here F is the set of final states].
	*/
	std::vector<std::vector<int>> marked_state_pairs;
	std::vector<std::vector<int>> non_marked_state_pairs;
	
	for (auto j = 0; j != states.size(); j++)//row
	{
		for (auto i = 0; i != states.size(); i++) //coluna
		{
			if (states.at(j) != states.at(i))
			{
				if (find(final.begin(), final.end(), j) != final.end())
				{
					if (find(final.begin(), final.end(), i) == final.end())
					{
						std::vector<int> conjunto = { j,i };
						if (find(marked_state_pairs.begin(), marked_state_pairs.end(), conjunto) == marked_state_pairs.end()) //se o conjunto não existir no vetor de marcados então metemos no vetor de marcados o novo pair {i,j}
						{
							marked_state_pairs.push_back({ j,i });
						}
					} 
				}
				else
				{
					if (find(final.begin(), final.end(), i) != final.end())
					{
						std::vector<int> conjunto = { j,i };
						if (find(marked_state_pairs.begin(), marked_state_pairs.end(), conjunto) == marked_state_pairs.end()) //se o conjunto não existir no vetor de marcados então metemos no vetor de marcados o novo pair {i,j}
						{
							marked_state_pairs.push_back({ j,i });
						}
					}
				}
			}
			else
			{
				break;
			}
		}	
	}

	/*
	*Step 3 − Repeat this step until we cannot mark anymore states −
	*If there is an unmarked pair(Qi, Qj), mark it if the pair{ δ(Qi, A), δ(Qi, A) } is marked for some input alphabet.
	*/
	

	minimization(marked_state_pairs, non_marked_state_pairs);

	/* Filling the states vector for the new minimized automato */
	
	for(auto i=0; i!= non_marked_state_pairs.size(); i++)
	std::sort(non_marked_state_pairs.at(i).begin(), non_marked_state_pairs.at(i).end());

	std::vector<std::vector<int>> minimized_states= non_marked_state_pairs;

	/* Fill the <minimized_states> with the remaining states of the automato. */
	
	for (auto i = 0; i != states.size(); i++) // row
	{
		for (auto j = 0; j != non_marked_state_pairs.size(); j++)
		{
			if (find(non_marked_state_pairs.at(j).begin(), non_marked_state_pairs.at(j).end(), i) != non_marked_state_pairs.at(j).end())
			{
				break;
			}
			else 
			{
				if (j == non_marked_state_pairs.size() - 1)
				{
					minimized_states.push_back({ i });
				}
			}
		}
	}

	if (minimized_states.size() == 0)
	{
		system("cls");
		std::cout << "Automato is already minimized!" << std::endl;
		return false;
	}
	
	/* Step 4 − Combine all the unmarked pair(Qi, Qj) and make them a single state in the reduced DFA. */
	
	int flag = 0;
	int proximo = 1;
	reuniao(minimized_states.at(0), proximo, minimized_states, flag);
	
	auto troll = 0;
	/* Step 5 - Fill the transitions map for the new minimized automato*/

	std::map<std::pair<int, int>, std::vector<int>> minimized_DFA_transitions;
	std::vector<int> tem;
	int c;
	for (auto q = 0; q != minimized_states.size(); q++)
	{
		for (auto w = 0; w != minimized_states.at(q).size(); w++)
		{
			for (auto e = 0; e != events.size(); e++)
			{
				if (transitions.count({ minimized_states.at(q).at(w),e }) == 1)
				{
					for (auto x = 0; x != transitions.at({ minimized_states.at(q).at(w),e }).size(); x++)
					{
						tem.push_back(transitions.at({ minimized_states.at(q).at(w),e }).at(x));
					}

					for (auto r = 0; r != minimized_states.size(); r++)
					{
						for (auto t = 0; t != minimized_states.at(r).size(); t++)
						{
							for (auto y = 0; y != tem.size(); y++)
							{
								if (tem.at(y) == minimized_states.at(r).at(t))
									c = r;
							}
						}
					}

					if (minimized_DFA_transitions.count({ q,e }) != 1)
					{
						minimized_DFA_transitions[{ q, e }].push_back(c);
						tem = std::vector<int>();
					}
				}
			}
		}
	}
	
	/* Step 6 - read the transitions map and fill the events vector with the events in map*/
	std::vector<int> events_temp;
	
	for (auto &entry : minimized_DFA_transitions) //PRINT da TABELA de TRANSIÇÕES
	{
		if (find(events_temp.begin(), events_temp.end(), entry.first.second) == events_temp.end())
		{
			events_temp.push_back(entry.first.second);
		}
	}
	std::sort(events_temp.begin(), events_temp.end()); //Por os eventos por ordem
	
	/* Step 7 - Create the vector that contains the final states */
	int initial_temp;

	for (auto i = 0; i < minimized_states.size(); i++)
	{
		for (auto j = 0; j < minimized_states.at(i).size(); j++)
		{
			if (find(minimized_states.at(i).begin(), minimized_states.at(i).end(), initial) != minimized_states.at(i).end())
			{
				initial_temp = i;
			}
		}
	}
	
	/* Step 8 - Create the vector that contains the final states */

	std::vector<int> final_temp;

	for (auto i = 0; i < minimized_states.size(); i++)
	{
		for (auto j = 0; j < minimized_states.at(i).size(); j++)
		{
			if ( find(final.begin(),final.end(), minimized_states.at(i).at(j) ) != final.end() )
			{
				if (find(final_temp.begin(), final_temp.end(), i) == final_temp.end())
				{
					final_temp.push_back(i);
				}
			}
		}
	}
	
	/* Step 8 - Pass the information through the parse function and update the automato with the new minimized automato */
	std::stringstream stringautomato;
	stringautomato = stringAutomatoForDFA(minimized_states, events_temp, initial_temp, final_temp, minimized_DFA_transitions);

	parseAutomato(stringautomato);
	return true;
}

void Automato::minimization(std::vector<std::vector<int>>& vect, std::vector<std::vector<int>>& nonvect)
{
	for (auto j = 0; j != states.size(); j++) // row
	{
		for (auto i = 0; i != states.size(); i++) //coluna
		{
			if (states.at(j) != states.at(i))
			{
				for (auto e = 0; e != events.size(); e++)
				{
					if ((transitions.count({ j,e }) == 1) && (transitions.count({ i,e }) == 1) && (transitions.at({ j,e }) != transitions.at({ i,e })))
					{
						std::vector<int> conjunto = { transitions.at({ i,e }).at(0), transitions.at({ j,e }).at(0) };
						std::vector<int> conjunto_inverso = { transitions.at({ j,e }).at(0), transitions.at({ i,e }).at(0) };

						if (find(vect.begin(), vect.end(),conjunto) != vect.end() || find(vect.begin(), vect.end(), conjunto_inverso) != vect.end()) 
						{	
							conjunto = { j,i };
							if (find(vect.begin(), vect.end(), conjunto) == vect.end())
							{
								vect.push_back({ j,i });
								minimization(vect, nonvect);
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	std::vector<int> temp;
	for (auto j = 0; j != states.size(); j++) // row
	{
		for (auto i = 0; i != states.size(); i++) //coluna
		{
			if (states.at(j) != states.at(i))
			{
				temp= { j,i };
				if (find(vect.begin(), vect.end(), temp) == vect.end())
				{
					if (find(nonvect.begin(), nonvect.end(), temp) == nonvect.end())
					{
						nonvect.push_back({ j,i });
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}

void Automato::productDFAteste(int chosen_automato_A, int chosen_automato_B,std::vector<Automato>& automatos)
{
	std::cout << "Product of automato a and b" << std::endl;
	std::cout << "Checking Automato A..." << std::endl;

	if (automatos.at(chosen_automato_A).checkDFANFA(1) == false)
	{
		std::cout << "Automato A must be DFA" << std::endl;
		return;
	}
	
	if (automatos.at(chosen_automato_A).checkAccessibility(1) == false)
	{
		std::cout << "Automato A must be Accessible." << std::endl;
		return;
	}

	if (automatos.at(chosen_automato_A).checkCoAccessibility(1) == false)
	{
		std::cout << "Automato A must be CoAccessible." << std::endl;
		return;
	}

	std::cout << "Checking Automato B..." << std::endl;

	if (automatos.at(chosen_automato_B).checkDFANFA(1) == false)
	{
		std::cout << "Automato B must be DFA" << std::endl;
		return;
	}
	
	if (automatos.at(chosen_automato_B).checkAccessibility(1) == false)
	{
		std::cout << "Automato B must be Accessible." << std::endl;
		return;
	}

	if (automatos.at(chosen_automato_B).checkCoAccessibility(1) == false)
	{
		std::cout << "Automato B must be CoAccessible." << std::endl;
		return;
	}

	productDFA(chosen_automato_A, chosen_automato_B,automatos);
}

void Automato::parallelDFAteste(int chosen_automato_A, int chosen_automato_B,std::vector<Automato>& automatos) /* devemos verificar acessibilidade e coacessibilidade?*/
{
	std::cout << "Parallel of automato a and b" << std::endl;

	parallelDFA(chosen_automato_A,chosen_automato_B, automatos);
}

void Automato::productDFA(int chosen_automato_A, int chosen_automato_B, std::vector<Automato> automatos) // Esta função precisa de receber o index do automato, não sei como
{
	std::vector<std::string> states_product;
	std::vector<std::string> events_product;
	std::map<std::pair<int, int>, std::vector<int>> transitions_product;
	int initial_product;
	std::vector<int> final_product;

	/* Intersect events from the chosen automatos A, B */
	if (automatos.at(chosen_automato_A).events.size() >= automatos.at(chosen_automato_B).events.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_A).events.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_B).events.size(); i++)
			{
				if (automatos.at(chosen_automato_A).events.at(j) == automatos.at(chosen_automato_B).events.at(i))
				{
					events_product.push_back(automatos.at(chosen_automato_A).events.at(j));
				}
			}
		}
	}
	else if (automatos.at(chosen_automato_A).events.size() < automatos.at(chosen_automato_B).events.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_B).events.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_A).events.size(); i++)
			{
				if (automatos.at(chosen_automato_B).events.at(j) == automatos.at(chosen_automato_A).events.at(i))
				{
					events_product.push_back(automatos.at(chosen_automato_B).events.at(j));
				}
			}
		}
	}
	
	std::sort(events_product.begin(), events_product.end());

	/* Check witch automato has more events then the <events_product> */
	
	for (auto i = 0; i != automatos.at(chosen_automato_A).events.size(); i++)
	{
		if(find(events_product.begin(), events_product.end(), automatos.at(chosen_automato_A).events.at(i)) == events_product.end())
		{
			/* delete from the transition map this event in order to clean the automato */
			for (auto j=0; j!=automatos.at(chosen_automato_A).states.size(); j++)
			{
				if (automatos.at(chosen_automato_A).transitions.count({ j,i }) == 1)
				{
					automatos.at(chosen_automato_A).transitions.erase({ j,i });
				}
			}
		}
	}

	for (auto i = 0; i != automatos.at(chosen_automato_B).events.size(); i++)
	{
		if (find(events_product.begin(), events_product.end(), automatos.at(chosen_automato_B).events.at(i)) == events_product.end())
		{
			/* delete from the transition map this event in order to clean the automato */
			for (auto j = 0; j != automatos.at(chosen_automato_B).states.size(); j++)
			{
				if (automatos.at(chosen_automato_B).transitions.count({ j,i }) == 1)
				{
					automatos.at(chosen_automato_B).transitions.erase({ j,i });
				}
			}
		}
	}
	
	/* Check if the two automatos are accessible and coaccessible, if not convert them! */
	automatos.at(chosen_automato_A).checkAccessibility(0);
	automatos.at(chosen_automato_A).checkCoAccessibility(0);
	automatos.at(chosen_automato_B).checkAccessibility(0);
	automatos.at(chosen_automato_B).checkCoAccessibility(0);

	/* Create the <states_product> and the <transitions_product> */ 
	if (automatos.at(chosen_automato_A).states.size() >= automatos.at(chosen_automato_B).states.size())
	{
		for (auto i = 0; i != automatos.at(chosen_automato_A).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_B).states.size(); j++)
			{
				states_product.push_back("(" + automatos.at(chosen_automato_A).states.at(i) + "," + automatos.at(chosen_automato_B).states.at(j) + ")");
			}
		}

		for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
			{
				for (auto e = 0; e != events_product.size(); e++)
				{
					if (automatos.at(chosen_automato_A).transitions.count({ j,e }) == 1)
					{
						if (automatos.at(chosen_automato_B).transitions.count({ i,e }) == 1)
						{
							std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
							auto it = find(states_product.begin(), states_product.end(), temp1);
							if (it != states_product.end())
							{
								for (auto v = 0; v != automatos.at(chosen_automato_A).transitions.at({ j,e }).size(); v++)
								{
									for (auto s = 0; s != automatos.at(chosen_automato_B).transitions.at({ i,e }).size(); s++)
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,e }).at(v)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,e }).at(s)) + ")";
										auto its = find(states_product.begin(), states_product.end(), temp2);
										if (its != states_product.end())
										{
											transitions_product[{(it - states_product.begin()), e}].push_back(its - states_product.begin());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (automatos.at(chosen_automato_A).states.size() < automatos.at(chosen_automato_B).states.size())
	{
		for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
			{
				states_product.push_back("(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")");
			}
		}

		for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
			{
				for (auto e = 0; e != events_product.size(); e++)
				{
					if (automatos.at(chosen_automato_A).transitions.count({ j,e }) == 1)
					{
						if (automatos.at(chosen_automato_B).transitions.count({ i,e }) == 1)
						{
							std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
							auto it = find(states_product.begin(), states_product.end(), temp1);
							if (it != states_product.end())
							{
								for (auto v = 0; v != automatos.at(chosen_automato_B).transitions.at({ i,e }).size(); v++)
								{
									for (auto s = 0; s != automatos.at(chosen_automato_A).transitions.at({ j,e }).size(); s++)
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,e }).at(s)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,e }).at(v)) + ")";
										auto its = find(states_product.begin(), states_product.end(), temp2);
										if (its != states_product.end())
										{
											transitions_product[{(it - states_product.begin()), e}].push_back(its- states_product.begin());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/* Choose the initial_product state */
	std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).initial) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).initial) + ")";
	auto its = find(states_product.begin(), states_product.end(), temp);

	if (its != states_product.end())
	{
		initial_product = (its - states_product.begin());
	}
	
	/* Choose the final_product state */

	if (automatos.at(chosen_automato_A).final.size() >= automatos.at(chosen_automato_B).final.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_A).final.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_B).final.size(); i++)
			{
				std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).final.at(j)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).final.at(i)) + ")";
				auto its = find(states_product.begin(), states_product.end(), temp);

				if (its != states_product.end())
				{
					final_product.push_back(its - states_product.begin());
				}
			}
		}
	}
	else if (automatos.at(chosen_automato_A).final.size() < automatos.at(chosen_automato_B).final.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_B).final.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_A).final.size(); i++)
			{
				std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).final.at(i)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).final.at(j)) + ")";
				auto its = find(states_product.begin(), states_product.end(), temp);

				if (its != states_product.end())
				{
					final_product.push_back(its - states_product.begin());
				}
			}
		}
	}

	clearAutomat();

	states= states_product;
	events= events_product;
	transitions= transitions_product;
	initial= initial_product;
	final= final_product;
	
	printAutomato(std::cout);
}

void Automato::parallelDFA(int chosen_automato_A, int chosen_automato_B, std::vector<Automato> automatos) // Esta função precisa de receber o index do automato, não sei como
{
	std::vector<std::string> states_parallel;
	std::vector<std::string> events_parallel;
	std::map<std::pair<int, int>, std::vector<int>> transitions_parallel;
	int initial_parallel;
	std::vector<int> final_parallel;

	/* reunion events from the chosen automatos A, B */
	if (automatos.at(chosen_automato_A).events.size() >= automatos.at(chosen_automato_B).events.size())
	{	
		events_parallel = automatos.at(chosen_automato_A).events;

		for (auto i = 0; i != automatos.at(chosen_automato_B).events.size(); i++)
		{
			if(find(events_parallel.begin(), events_parallel.end(), automatos.at(chosen_automato_B).events.at(i)) == events_parallel.end())
			{
				events_parallel.push_back(automatos.at(chosen_automato_B).events.at(i));
			}
		}
	}
	else if (automatos.at(chosen_automato_A).events.size() < automatos.at(chosen_automato_B).events.size())
	{
		events_parallel = automatos.at(chosen_automato_B).events;

		for (auto i = 0; i != automatos.at(chosen_automato_A).events.size(); i++)
		{
			if (find(events_parallel.begin(), events_parallel.end(), automatos.at(chosen_automato_A).events.at(i)) == events_parallel.end())
			{
				events_parallel.push_back(automatos.at(chosen_automato_A).events.at(i));
			}
		}
	}

	std::sort(events_parallel.begin(), events_parallel.end());

	/* Check if the two automatos are accessible and coaccessible, if not convert them! */
	//automatos.at(chosen_automato_A).checkAccessibility(0);
	//automatos.at(chosen_automato_A).checkCoAccessibility(0);
	//automatos.at(chosen_automato_B).checkAccessibility(0);
	//automatos.at(chosen_automato_B).checkCoAccessibility(0);

	/* Create the <states_product> and the <transitions_product> */
	if (automatos.at(chosen_automato_A).states.size() >= automatos.at(chosen_automato_B).states.size())
	{
		for (auto i = 0; i != automatos.at(chosen_automato_A).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_B).states.size(); j++)
			{
				states_parallel.push_back("(" + automatos.at(chosen_automato_A).states.at(i) + "," + automatos.at(chosen_automato_B).states.at(j) + ")");
			}
		}

		for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
			{
				for (auto e = 0; e != events_parallel.size(); e++)
				{
					auto w = find(automatos.at(chosen_automato_A).events.begin(), automatos.at(chosen_automato_A).events.end(), events_parallel.at(e));
					auto q = find(automatos.at(chosen_automato_B).events.begin(), automatos.at(chosen_automato_B).events.end(), events_parallel.at(e));
					int eventoA = w - automatos.at(chosen_automato_A).events.begin();
					int eventoB = q - automatos.at(chosen_automato_B).events.begin();

					if (automatos.at(chosen_automato_A).transitions.count({ j,eventoA }) == 1)
					{
						if (automatos.at(chosen_automato_B).transitions.count({ i,eventoB }) == 1)
						{
							std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
							auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
							if (it != states_parallel.end())
							{
								for (auto v = 0; v != automatos.at(chosen_automato_A).transitions.at({ j,e }).size(); v++)
								{
									for (auto s = 0; s != automatos.at(chosen_automato_B).transitions.at({ i,e }).size(); s++)
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,e }).at(v)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,e }).at(s)) + ")";
										auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
										if (its != states_parallel.end())
										{
											transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
										}
									}
								}
							}
						}
						else
						{
							if (find(automatos.at(chosen_automato_B).events.begin(), automatos.at(chosen_automato_B).events.end(), events_parallel.at(e)) == automatos.at(chosen_automato_B).events.end())
							{
								if (automatos.at(chosen_automato_A).transitions.count({ j,eventoA }) == 1)
								{
									for (auto v = 0; v != automatos.at(chosen_automato_A).transitions.at({ j,eventoA }).size(); v++)
									{
										std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
										auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
										if (it != states_parallel.end())
										{
											std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,eventoA }).at(v)) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
											auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
											if (its != states_parallel.end())
											{
												transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
											}
										}
									}
								}
							}
						}
					}
					else
					{
						if (find(automatos.at(chosen_automato_A).events.begin(), automatos.at(chosen_automato_A).events.end(), events_parallel.at(e)) == automatos.at(chosen_automato_A).events.end())
						{
							if (automatos.at(chosen_automato_B).transitions.count({ i,eventoB }) == 1)
							{
								for (auto v = 0; v != automatos.at(chosen_automato_B).transitions.at({ i,eventoB }).size(); v++)
								{
									std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
									auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
									if (it != states_parallel.end())
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,eventoB }).at(v)) + ")";
										auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
										if (its != states_parallel.end())
										{
											transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (automatos.at(chosen_automato_A).states.size() < automatos.at(chosen_automato_B).states.size())
	{
		for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
		{
			for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
			{
				states_parallel.push_back("(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")");
			}
		}

		for (auto j = 0; j != automatos.at(chosen_automato_A).states.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_B).states.size(); i++)
			{
				for (auto e = 0; e != events_parallel.size(); e++)
				{
					auto w = find(automatos.at(chosen_automato_A).events.begin(), automatos.at(chosen_automato_A).events.end(), events_parallel.at(e));
					auto q = find(automatos.at(chosen_automato_B).events.begin(), automatos.at(chosen_automato_B).events.end(), events_parallel.at(e));
					int eventoA = w - automatos.at(chosen_automato_A).events.begin();
					int eventoB = q - automatos.at(chosen_automato_B).events.begin();

					if (automatos.at(chosen_automato_B).transitions.count({ i,eventoB }) == 1) 
					{
						if  (automatos.at(chosen_automato_A).transitions.count({ j,eventoA }) == 1) 
						{
							std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
							auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
							if (it != states_parallel.end())
							{
								for (auto v = 0; v != automatos.at(chosen_automato_B).transitions.at({ i,e }).size(); v++)
								{
									for (auto s = 0; s != automatos.at(chosen_automato_A).transitions.at({ j,e }).size(); s++)
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,e }).at(s)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,e }).at(v)) + ")";
										auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
										if (its != states_parallel.end())
										{
											transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
										}
									}
								}
							}
						}
						else
						{
							if (find(automatos.at(chosen_automato_A).events.begin(), automatos.at(chosen_automato_A).events.end(), events_parallel.at(e)) == automatos.at(chosen_automato_A).events.end())
							{
								if (automatos.at(chosen_automato_B).transitions.count({ i,eventoB }) == 1)
								{
									for (auto v = 0; v != automatos.at(chosen_automato_B).transitions.at({ i,eventoB }).size(); v++)
									{
										std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
										auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
										if (it != states_parallel.end())
										{
											std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).transitions.at({ i,eventoB }).at(v)) + ")";
											auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
											if (its != states_parallel.end())
											{
												transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
											}
										}
									}
								}
							}
						}
					}
					else
					{
						if (find(automatos.at(chosen_automato_B).events.begin(), automatos.at(chosen_automato_B).events.end(), events_parallel.at(e)) == automatos.at(chosen_automato_B).events.end())
						{
							if (automatos.at(chosen_automato_A).transitions.count({ j,eventoA }) == 1)
							{
								for (auto v = 0; v != automatos.at(chosen_automato_A).transitions.at({ j,eventoA }).size(); v++)
								{
									std::string temp1 = "(" + automatos.at(chosen_automato_A).states.at(j) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
									auto it = find(states_parallel.begin(), states_parallel.end(), temp1);
									if (it != states_parallel.end())
									{
										std::string temp2 = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).transitions.at({ j,eventoA }).at(v)) + "," + automatos.at(chosen_automato_B).states.at(i) + ")";
										auto its = find(states_parallel.begin(), states_parallel.end(), temp2);
										if (its != states_parallel.end())
										{
											transitions_parallel[{(it - states_parallel.begin()), e}].push_back(its - states_parallel.begin());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/* Choose the initial_product state */
	std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).initial) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).initial) + ")";
	auto its = find(states_parallel.begin(), states_parallel.end(), temp);

	if (its != states_parallel.end())
	{
		initial_parallel = (its - states_parallel.begin());
	}

	/* Choose the final_product state */

	if (automatos.at(chosen_automato_A).final.size() >= automatos.at(chosen_automato_B).final.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_A).final.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_B).final.size(); i++)
			{
				std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).final.at(j)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).final.at(i)) + ")";
				auto its = find(states_parallel.begin(), states_parallel.end(), temp);

				if (its != states_parallel.end())
				{
					final_parallel.push_back(its - states_parallel.begin());
				}
			}
		}
	}
	else if (automatos.at(chosen_automato_A).final.size() < automatos.at(chosen_automato_B).final.size())
	{
		for (auto j = 0; j != automatos.at(chosen_automato_B).final.size(); j++)
		{
			for (auto i = 0; i != automatos.at(chosen_automato_A).final.size(); i++)
			{
				std::string temp = "(" + automatos.at(chosen_automato_A).states.at(automatos.at(chosen_automato_A).final.at(i)) + "," + automatos.at(chosen_automato_B).states.at(automatos.at(chosen_automato_B).final.at(j)) + ")";
				auto its = find(states_parallel.begin(), states_parallel.end(), temp);

				if (its != states_parallel.end())
				{
					final_parallel.push_back(its - states_parallel.begin());
				}
			}
		}
	}

	clearAutomat();

	states = states_parallel;
	events = events_parallel;
	transitions = transitions_parallel;
	initial = initial_parallel;
	final = final_parallel;

	printAutomato(std::cout);
}