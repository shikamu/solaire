#pragma once

struct ScoreRow
{
	const unsigned int ID;	//this is either the agent id or the group id based on whether this row is a player or a team row
	const int Score;
	const unsigned int Kills;
	const unsigned int Deaths;
	const float Accuracy;

	const char IsTeam;//0 for player data, anything else for team score

	ScoreRow(const unsigned int id, const int score, const unsigned int kills, const unsigned int deaths, const float accuracy, const char team) : ID(id), Score(score), Kills(kills), Deaths(deaths), Accuracy(accuracy), IsTeam(team)
	{
	}
};

