/**
 * @file main.cpp
 * @brief Application Entry Point
 *
 */

/* Application Entry Point */
int main()
{
    /* Starts Profile Session */
    H_PROFILE_BEGIN_SESSION("Application Profile", "profile_results.json");

    {
        /* Profiles the Main Function */
        H_PROFILE_FUNCTION();

        /* Initialize Logger */
        Horus::Logger::init();

        {
            /* Profiles the Logging ShowCase Scope */
            H_PROFILE_SCOPE("Logging Showcase");

            /* CMake Configured File Defines */
            H_INFO("PROJECT NAME => {}", PROJECT_NAME);
            H_INFO("PROJECT VERSION => {}", PROJECT_VERSION);
            H_INFO("PROJECT DESCRIPTION => {}", PROJECT_DESCRIPTION);
            H_INFO("PROJECT URL => {}", PROJECT_HOMEPAGE_URL);

            /* Some Log Utils */
            H_TRACE("That's a trace!");
            H_DEBUG("That's a debug.");
            H_INFO("That's an info.");
            H_WARN("That's a warning.");
            H_ERROR("That's an error.");
            H_CRITICAL("That's a critical.");
        }
    }

    /* Ends Profile Session */
    H_PROFILE_END_SESSION();

    /* Assertions Examples */
    H_ASSERT(true);                                  // Assertion with default message
    H_ASSERTM(true, "Assertion with {}", "message"); // Assertion with user defined message

    return 0;
}