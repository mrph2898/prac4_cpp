# prac4_cpp
Lexical And Syntax Analyser
"analyse.sh" compiles lexer.cpp, test it and make coverage for it (if it was "run" parameter), then
             compiles parser.cpp, test it("run" parameter), make coverage for it("lcov" parameter)
             and run firefox(html-file of coverage, if "web" was as parameter)
 Example: ./analyse.sh run lcov web
"test_lexer.sh" compiles lexer.cpp, test it, make coverage for it (if it was "l" parameter or "lcov")
                and run firefox(html-file of coverage, if "web" was as parameter)
 Example: ./test_lexer.sh run l web

All object and executible files will be in ./ObiFiles, test results will be in ./TestResults.
