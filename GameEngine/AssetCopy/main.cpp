#include <cstdio>
#include <cstdlib>
#include <string>


int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        printf("FORMAT:");
        printf(" -<ProjectName> -<Configuration>");

        return 0;
    }

    std::string projectName = argv[1];
    std::string config = argv[2];

    projectName = std::string(projectName.begin() + 1, projectName.end());
    config = std::string(config.begin() + 1, config.end());

    std::string copyEngineRequiredAssetsCommand = std::string("Xcopy /E /I /y ") + R"(.\Assets\EngineRequired )" + R"(.\x64\)" + config + R"(\Assets\)";
    std::string copyProjectAssetsCommand = std::string("Xcopy /E /I /y ") + R"(.\Assets\)" + projectName + " " + R"(.\x64\)" + config + R"(\Assets\)";

    int result = std::system(copyEngineRequiredAssetsCommand.c_str());

    if (result == 0) {
        printf("Required engine assets copied successfully!");
    }
    else {
        printf((std::string("Xcopy failed with error code: ") + std::to_string(result)).c_str());
    }

    printf("\n");

    result = std::system(copyProjectAssetsCommand.c_str());

    if (result == 0) {
        printf("Project assets copied successfully!");
    }
    else {
        printf((std::string("Xcopy failed with error code: ") + std::to_string(result)).c_str());
    }

    return 0;
}