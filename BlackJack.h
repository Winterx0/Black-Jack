#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <thread>

using namespace std;

////////////////////////////////////////////////////////////////////////////
class OpeningMessage
{
public:
	void message()
	{
		cout << "Welcome to Black Jack!\n";
		cout << "In this game you can pick a number of players. [1-7]" << endl;
		cout << "Then you pick names for all of the players." << endl;
		cout << "You have the option to hit to get more cards and points for every player.";
		cout << "Whoever gets 21 pts wins." << endl;
		cout << "There is a score sheet I/O file available to keep track of your scores." << endl;
		cout << "When you finish the game you can choose to play again, view the score sheet, or exit the program." << endl;
		cout << "Thanks for playing!\n";
		system("pause");
		system("CLS");
	}
	
};

////////////////////////////////////////////////////////////////////////////
class Card
{
public:
	enum rank {ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
	enum suit {CLUBS, SPADES, DIAMONDS, HEARTS};

	//overloading operator to send Card object to standard output
	friend ostream& operator<<(ostream& os, const Card& aCard);

	Card(rank r = ACE, suit s = SPADES, bool ifu = true);

	//returns value of card 1-11
	int GetValue() const;

	//flips a card, if face up, becomes face down vice-versa
	void flip();

private:
	rank m_rank;
	suit m_suit;
	bool m_isFaceUp;
};

Card::Card(rank r, suit s, bool ifu) : m_rank(r), m_suit(s), m_isFaceUp(ifu) { }

int Card::GetValue() const
{
	//if a card is face down its value is 0
	int value = 0;
	if (m_isFaceUp)
	{
		//value is number showing on card
		value = m_rank;
		//value is 10 for face cards
		if (value > 10)
		{
			value = 10;
		}
	}
	return value;
}

void Card::flip()
{
	m_isFaceUp = !(m_isFaceUp);
}

////////////////////////////////////////////////////////////////////////////
class Hand
{
public:
	Hand();
	virtual ~Hand();

	//adds card to the hand
	void Add(Card* pCard);

	//clears hand of all cards
	void Clear();

	//gets hand total value, treats aces as 1 or 11
	int GetTotal() const;

protected:
	vector<Card*> m_Cards;
};

Hand::Hand()
{
	m_Cards.reserve(7);
}

Hand::~Hand()
{
	Clear();
}

void Hand::Add(Card* pCard)
{
	m_Cards.push_back(pCard);
}

void Hand::Clear()
{
	//iterate through vector freeing all memory on the heap
	vector<Card*>::iterator iter = m_Cards.begin();
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		delete *iter;
		*iter = 0;
	}

	//clear vector of pointers
	m_Cards.clear();
}

int Hand::GetTotal() const
{
	//if no cards in hand return 0
	if (m_Cards.empty())
	{
		return 0;
	}
	//if first card has value of 0 card is face down/return 0
	if (m_Cards[0]->GetValue() == 0)
	{
		return 0;
	}
	//add card values treat each ace as 1
	int total = 0;
	vector<Card*>::const_iterator iter;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		total += (*iter)->GetValue();
	}
	//determine if hand containd an ace
	bool containsAce = false;
	for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
	{
		if ((*iter)->GetValue() == Card::ACE)
		{
			containsAce = true;
		}
	}

	//if hand contains ace and total is low enough treat ace as 11
	if (containsAce && total <= 11)
	{
		//add only 10 since 1 was already added for the ace
		total += 10;
	}

	return total;
}

////////////////////////////////////////////////////////////////////////////
class GenericPlayer : public Hand
{
	friend ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

public:
	GenericPlayer(const string& name = "");
	virtual ~GenericPlayer();

	//indicates whether or not generic player wants to keep hitting
	virtual bool isHitting() const = 0;

	//returns whether generic player has busted- has a total greater than 21
	bool isBusted() const;

	//announces generic player busts
	void bust() const;

protected:
	string m_name;
};

GenericPlayer::GenericPlayer(const string& name) : m_name(name) { }

GenericPlayer::~GenericPlayer() { }

bool GenericPlayer::isBusted() const
{
	return (GetTotal() > 21);
}

void GenericPlayer::bust() const
{
	cout << m_name << " Busts.\n";
}

////////////////////////////////////////////////////////////////////////////
//class derived from generic player
class Player : public GenericPlayer
{
public:
	Player(const string& name = "");
	virtual ~Player();

	//returns whether or not player wants another hit
	virtual bool isHitting() const;

	//announces the player wins
	void win();

	//announces the player loses
	void lose() const;

	//announces the player pushes
	void push() const;

	//read
	int score = 0;
	bool read(ifstream& afile)
	{

		if (afile.eof())
		{
			return false;
		}
		afile >> m_name  >> score;
		return true;
	}

	void write(ofstream& afile) {
		 afile << m_name << " " << score << endl;
	}
};

Player::Player(const string& name) : GenericPlayer(name) { }

Player::~Player() { }

bool Player::isHitting() const
{
	cout << m_name << " do you want another hit? [Y/N] " << endl;
	char response;
	cin >> response;

	return toupper(response) != 'N';
}

void Player::win()
{
	score++;
	cout << m_name << " wins\n";
}

void Player::lose() const
{
	cout << m_name << " loses.\n";
}

void Player::push() const
{
	cout << m_name << " pushes.\n";
}

////////////////////////////////////////////////////////////////////////////
class House : public GenericPlayer
{
public:
	House(const string& name = "");
	virtual ~House();

	//indicates whether house is hitting - will always hit 16 or less
	virtual bool isHitting() const;

	//flips over first card
	void flipFirstCard();
};

House::House(const string& name) : GenericPlayer(name) { }

House::~House() { }

bool House::isHitting() const
{
	return (GetTotal() <= 16);
}

void House::flipFirstCard()
{
	if (!m_Cards.empty())
	{
		m_Cards[0]->flip();
	}
	else
	{
		cout << "No card to flip!\n";
	}
}

////////////////////////////////////////////////////////////////////////////
class Deck : public Hand
{
public:
	Deck();
	virtual ~Deck();

	//create standard deck of 52 cards
	void populate();

	//shuffle cards
	void shuffle();

	//deal one card to a hand
  bool deal(Hand& aHand);

	//give additional cards to a generic player
	void additionalCards(GenericPlayer& aGenericPlayer);
};

Deck::Deck()
{
	m_Cards.reserve(52);
	populate();
}

Deck::~Deck() { }

void Deck::populate()
{
	Clear();

	//create standard deck
	for (int s = Card::CLUBS; s <= Card::SPADES; ++s)
	{
		for (int r = Card::ACE; r <= Card::KING; ++r)
		{
			Add(new Card(static_cast<Card::rank>(r), static_cast<Card::suit>(s)));
		}
	}
}

void Deck::shuffle()
{
	random_shuffle(m_Cards.begin(), m_Cards.end());
}

bool Deck::deal(Hand& aHand)
{
	if (!m_Cards.empty())
	{
		aHand.Add(m_Cards.back());
		m_Cards.pop_back();
		return true;
	}
	else
	{
		cout << "Out of cards, unable to deal.\n";
		return false;
	}
}

void Deck::additionalCards(GenericPlayer& aGenericPlayer)
{
	cout << endl;

	//continue to deal as long as generic player isn't busted and wants another hit
	while (!(aGenericPlayer.isBusted()) && aGenericPlayer.isHitting())
	{
		if(!deal(aGenericPlayer)) break;
		cout << aGenericPlayer << endl;
		if (aGenericPlayer.isBusted())
		{
			aGenericPlayer.bust();
		}
	}
}

////////////////////////////////////////////////////////////////////////////
class Game
{
public:
	Game(const vector<string>& names);
	Game();
	~Game();

	//plays game
	void play();

	//opens menu options
	void choice();

	//adds score to fstream
	void writeScore();

	//displays scores to console from fstream
	void viewScore();

	//loads score
	void loadScore();



private:
	Deck m_deck;
	House m_house;
	vector<Player> m_players;

};

Game::Game(const vector<string>& names)
{
	//create a vector of players from a vector of names
	vector<string>::const_iterator pName;
	for (pName = names.begin(); pName != names.end(); ++pName)
	{
		m_players.push_back(Player(*pName));
	}

	//seed the random number generator
	srand(static_cast<unsigned int>(time(0)));
	m_deck.populate();
	m_deck.shuffle();
}

Game::Game()
{
	//create an emptz desk
	//seed the random number generator
	srand(static_cast<unsigned int>(time(0)));
	m_deck.populate();
	m_deck.shuffle();
}

Game::~Game() { }

void Game::play()
{
	//deal initial 2 cards to everyone
	vector<Player>::iterator pPlayer;
	for (int i = 0; i < 2; ++i)
	{
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
		{
			m_deck.deal(*pPlayer);
		}
		m_deck.deal(m_house);
	}

	//hide houses first card
	m_house.flipFirstCard();

	//display everyones hand
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
	{
		cout << *pPlayer << endl;
	}

	cout << m_house << endl;

	//deal additional cards to players
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
	{
		m_deck.additionalCards(*pPlayer);
	}

	//reveal houses first card
	m_house.flipFirstCard();
	cout << endl << m_house;

	//deal additional cards to house
	m_deck.additionalCards(m_house);
	if (m_house.isBusted())
	{
		//everyone still playing wins
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
		{
			if (!(pPlayer->isBusted()))
			{
				pPlayer->win();
			}
		}
	}
	else
	{
		//compare each player still playing to house
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
		{
			if (!pPlayer->isBusted())
			{
			
				if (pPlayer->GetTotal() <= m_house.GetTotal())
				{
					pPlayer->win();
				}
				else if (pPlayer->GetTotal() <= m_house.GetTotal())
				{
					pPlayer->lose();
				}
				else
				{
					pPlayer->push();
				}
			}
		}
	}

	//remove everyones cards
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
	{
		pPlayer->Clear();
	}
	m_house.Clear();
}

void Game::writeScore()
{
	string input;
	vector<Player>::iterator pPlayer;

	ofstream outputfile("ScoreSheet.txt");

		if (outputfile.is_open())
		{
			for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer)
			{
				pPlayer->write(outputfile);
			}
		}

		cout << "\nYou've added new players/scores to the Output file!" << endl;
		
}

void Game::viewScore()
{
	string name = "unknown";
	int score = 0;
	ifstream afile("ScoreSheet.txt");
	if (afile.is_open())
	{

		try {
				while (!afile.eof())
				{
					score = -1;
				
					afile >> name >> score;
					if (score < 0)
					{
						break;
					}
					cout << name << " : " << score << endl;
				}
		}
		catch (...)	{
		}
	}
	system("pause");
	system("CLS");
}

void Game::loadScore()
{
	string name = "unknown";
	int score = 0;
	ifstream afile("ScoreSheet.txt");
	if (afile.is_open())
	{
		m_players.clear();

		try {
			while (!afile.eof())
			{
				score = -1;

				afile >> name >> score;
				if (score < 0)
				{
					break;
				}
			//	Player *p = new Player(name);
				m_players.push_back(Player(name));
				cout << name << " : " << score << endl;
			}
		}
		catch (...) {
		}
	}
	system("pause");
	system("CLS");
}

//end of game menu option
void Game::choice()
{
	char respond;
	bool run = true;

	do
	{
		cout << "\n=========================================================" << endl;
		cout << "\nWould you like to [R]estart the game, [S]ave score, [V]iew the score sheet or [Q]uit?" << endl;
		cin >> respond;
		
		switch (toupper(respond))
		{
		case 'R': Game::play(); break;
		case 'S': Game::writeScore(); break;
		case 'V': Game::viewScore(); break;
		case 'Q': run = false; break;
		}
	} while (run);
}
