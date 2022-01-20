#include "BlackJack.h"

//function prototypes
ostream& operator<<(ostream& os, const Card& aCard);
ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

  void readPLayers()
  {
	  //pick number of players
	  int numPlayers = 0;
	while (numPlayers < 1 || numPlayers > 7)
	{
		cout << "How many players? [1-7] " << endl;
		cin >> numPlayers;
		break;
	}

	////////////////////////////////////////////////////////////////////////////
	//pick player names
	vector<string> names;
	string name;
	for (int i = 0; i < numPlayers; ++i)
	{
		cout << "Enter player name: " << endl;
		cin >> name;
		names.push_back(name);
	}
	cout << endl;
	}

	int main()
	{

		//opening message - game description
		OpeningMessage open;
		open.message();

		////////////////////////////////////////////////////////////////////////////
		

	////////////////////////////////////////////////////////////////////////////
	//the game play
	Game aGame;
	aGame.loadScore();
	aGame.play();
	aGame.choice();


	return 0;
}

////////////////////////////////////////////////////////////////////////////
//overloads operator so card object can be sent to cout
ostream& operator<<(ostream& os, const Card& aCard)
{
	const string RANKS[] = { "0", "ACE-", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
	const string SUITS[] = { "-CLUBS", "-DIAMONDS", "-HEARTS", "-SPADES"};

	if (aCard.m_isFaceUp)
	{
		os << RANKS[aCard.m_rank] << SUITS[aCard.m_suit];
	}
	else
	{
		os << "XX";
	}
	return os;
}

////////////////////////////////////////////////////////////////////////////
//overloads operator so Generic Player object can be sent to cout
ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer)
{
	os << aGenericPlayer.m_name << ":\t";

	vector<Card*>::const_iterator pCard;
	if (!aGenericPlayer.m_Cards.empty())
	{
		for (pCard = aGenericPlayer.m_Cards.begin(); pCard != aGenericPlayer.m_Cards.end(); ++pCard)
		{
			os << *(*pCard) << "\t";
		}
		if (aGenericPlayer.GetTotal() != 0)
		{
			cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	}
	else
	{
		os << "<empty>";
	}
	return os;
}