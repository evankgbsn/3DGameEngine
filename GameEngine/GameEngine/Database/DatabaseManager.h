#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../Utils/SingletonHelpers.h"
#include "../Utils/Logger.h"

#include <SQLiteCpp/SQLiteCpp.h>

#include <unordered_map>
#include <vector>
#include <tuple>

class DatabaseManager
{
public:

	struct TableColumn
	{
		std::string name;
		std::string dataType;
		std::vector<std::string> constraints;

		std::string ToString() const;
	};

	static void Initialize();

	static void Terminate();

	static void OpenDatabase(const std::string& databaseName, const std::string& password);

	static void CreateTable(const std::string& databaseName, const std::string& tableName, const std::vector<TableColumn>& columns);

	template<typename... Args>
	static void AddTableEntry(const std::string& databaseName, const std::string& tableName, const std::vector<std::string> columnNames, Args... args);

	template<typename T>
	static T GetDataFromTableRow(const std::string& databaseName, const std::string& tableName, const std::string& keyColumn, const std::string& key, const std::string& dataColumn);

	static void CloseDatabase(const std::string& databaseName);

private:

	friend class SingletonHelpers;

	DatabaseManager();

	~DatabaseManager();

	DatabaseManager(const DatabaseManager&) = delete;

	DatabaseManager& operator=(const DatabaseManager&) = delete;

	DatabaseManager(DatabaseManager&&) = delete;

	DatabaseManager& operator=(DatabaseManager&&) = delete;

	void Test();

	static DatabaseManager* instance;

	std::unordered_map<std::string, SQLite::Database*> openedDatabases;
};

// Helper function that uses a fold expression
template<typename... Args>
void BindAll(SQLite::Statement& statement, const Args&... args) {
	int index = 1; // The starting index for binding
	// The fold expression expands this for each argument
	(statement.bind(index++, args), ...);
}

template<typename ...Args>
inline void DatabaseManager::AddTableEntry(const std::string& databaseName, const std::string& tableName, const std::vector<std::string> columnNames, Args ...args)
{
	if (instance != nullptr)
	{
		const auto& database = instance->openedDatabases.find(databaseName);

		if (database != instance->openedDatabases.end())
		{
			//std::tuple<Args...> arguments(args...);

			std::string command = "INSERT INTO " + tableName + " (";


			std::string values = "(";
			for (unsigned int i = 0; i < columnNames.size(); i++)
			{
				if (i != 0)
				{
					command += ", ";
					values += ", ";
				}

				command += columnNames[i];
				values += "?";
			}

			command += ") VALUES " + values + ")";

			SQLite::Statement statement(*database->second, command);

			BindAll(statement, args...);

			try
			{
				statement.exec();
			}
			catch (const std::exception& e)
			{
				Logger::Log("Failed to add table entry Exception: " + std::string(e.what()), Logger::Category::Warning);
			}

			Logger::Log("Inserted data to table: " + tableName + " of database: " + databaseName, Logger::Category::Success);
		}
		else
		{
			Logger::Log("Database " + databaseName + " has not been opened. DatabaseManager::AddTableEntry", Logger::Category::Warning);
		}
		
	}
	else
	{
		Logger::Log("Calling DatabaseManager::AddTableEntry before DatabaseManager::Initialize", Logger::Category::Warning);
	}
}

template<typename T>
inline T DatabaseManager::GetDataFromTableRow(const std::string& databaseName, const std::string& tableName, const std::string& keyColumn, const std::string& key, const std::string& dataColumn)
{
	if (instance != nullptr)
	{
		const auto& database = instance->openedDatabases.find(databaseName);

		if (database != instance->openedDatabases.end())
		{
			std::string query = "SELECT * FROM " + tableName + " WHERE " + keyColumn + " = ?";
			SQLite::Statement select_query(*database->second, query.c_str());
			select_query.bind(1, key.c_str());
			select_query.executeStep();

			return select_query.getColumn(dataColumn.c_str());
		}
		else
		{
			Logger::Log("Database " + databaseName + " has not been opened. DatabaseManager::GetDataFromTableRow", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling DatabaseManager::GetDataFromTableRow before DatabaseManager::Initialize", Logger::Category::Warning);
	}
}

#endif // DATABASEMANAGER_H