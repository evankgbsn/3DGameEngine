#include "DatabaseManager.h"

DatabaseManager* DatabaseManager::instance = nullptr;

void DatabaseManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "DatabaseManager");

	instance->Test();
}

void DatabaseManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "DatabaseManager");
}

void DatabaseManager::OpenDatabase(const std::string& databaseName, const std::string& password)
{
	if (instance != nullptr)
	{
		const auto& openDatabase = instance->openedDatabases.find(databaseName);

		if (openDatabase == instance->openedDatabases.end())
		{
			// 1. Create or open the encrypted database file.
			// The file on disk ("player_data_encrypted.db") will be fully encrypted.
			SQLite::Database* db = new SQLite::Database("Assets/DB/" + databaseName + ".db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
			
			try
			{
				db->exec(("PRAGMA key = " + password).c_str());
			}
			catch (const std::exception& e)
			{
				Logger::Log("Incorrect password for database: " + databaseName, Logger::Category::Error);
				delete db;
				db = nullptr;
			}

			if (db != nullptr)
			{
				instance->openedDatabases[databaseName] = db;

				Logger::Log("Opened/Created encrypted database file: " + databaseName, Logger::Category::Success);
			}
		}
		else
		{
			Logger::Log("Database file: " + databaseName + " is already open.", Logger::Category::Success);
		}

		
	}
	else
	{
		Logger::Log("Calling DatabaseManager::OpenDatabase before DatabaseManager::Initialize", Logger::Category::Warning);
	}
}

void DatabaseManager::CreateTable(const std::string& databaseName, const std::string& tableName, const std::vector<TableColumn>& columns)
{
	if (instance != nullptr)
	{
		const auto& database = instance->openedDatabases.find(databaseName);

		if (database != instance->openedDatabases.end())
		{
			std::string command = "CREATE TABLE IF NOT EXISTS " + tableName + " (";

			for (unsigned int i = 0; i < columns.size(); i++)
			{
				if (i != 0)
				{
					command += ", ";
				}

				command += columns[i].ToString();
			}

			command += ")";

			try
			{
				database->second->exec(command.c_str());
				Logger::Log("Created table: " + tableName + " in database: " + databaseName, Logger::Category::Success);
			}
			catch (const std::exception& e)
			{
				Logger::Log("Failed to create table: " + tableName + " for database: " + databaseName + " Exception: " + e.what(), Logger::Category::Error);
			}
		}
		else
		{
			Logger::Log("Database: " + databaseName + " has not been opened. DatabaseManager::CreateTable");
		}
	}
	else
	{
		Logger::Log("Calling DatabaseManager::CreateTable before DatabaseManager::Initialize", Logger::Category::Warning);
	}
}

void DatabaseManager::CloseDatabase(const std::string& databaseName)
{
	if (instance != nullptr)
	{
		const auto& database = instance->openedDatabases.find(databaseName);

		if (database != instance->openedDatabases.end())
		{
			delete database->second;
			instance->openedDatabases.erase(database);
		}
		else
		{
			Logger::Log("Database: " + databaseName + " has not been opened. DatabaseManager::CloseDatabase");
		}
	}
	else
	{
		Logger::Log("Calling DatabaseManager::CloseDatabase before DatabaseManager::Initialize", Logger::Category::Warning);
	}
}

DatabaseManager::DatabaseManager()
{
	
}

DatabaseManager::~DatabaseManager()
{
	// Close all databases.
	for (const auto& database : openedDatabases)
	{
		delete database.second;
	}

	openedDatabases.clear();
}

void DatabaseManager::Test()
{
	OpenDatabase("NewDatabase", "password");

	TableColumn column0;
	column0.name = "ID";
	column0.dataType = "INTEGER";
	column0.constraints = { "PRIMARY KEY" };

	TableColumn column1;
	column1.name = "Username";
	column1.dataType = "TEXT";
	column1.constraints = { "NOT NULL", "UNIQUE" };

	TableColumn column2;
	column2.name = "Level";
	column2.dataType = "INTEGER";
	column2.constraints = { "NOT NULL" };

	TableColumn column3;
	column3.name = "Health";
	column3.dataType = "INTEGER";
	column3.constraints = { "NOT NULL" };

	CreateTable("NewDatabase", "PlayerData", { column0, column1, column2, column3 });

	AddTableEntry("NewDatabase", "PlayerData", std::vector<std::string>({ column0.name, column1.name, column2.name, column3.name }), 0, "player1", 100, 50);

	unsigned int health = GetDataFromTableRow<int>("NewDatabase", "PlayerData", "Username", "player1", "Health");

	Logger::Log("Player1 health = " + std::to_string(health));

	CloseDatabase("NewDatabase");
}

std::string DatabaseManager::TableColumn::ToString() const
{
	std::string ret;

	ret += name + " ";
	ret += dataType;

	for (const auto& constraint : constraints)
	{
		ret += " ";
		ret += constraint;
	}

	return ret;
}
