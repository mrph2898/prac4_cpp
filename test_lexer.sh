#!/bin/bash
CC="g++-7"
OBJDIR="./Objfiles"
TESTDIR="./Tests/Lex_tests"
TESTR="./TestsResults"
TESTRES="./TestsResults/Lex_results"
TESTVAL="./TestsResults/Lex_valgrind"
SW="./SourceCode/test_lexer.cpp"
OW="$OBJDIR/tl.o"
PW="$OBJDIR/tl"
OPTIONS_CC="-O2 -Wall -Werror -pedantic-errors -Wextra -std=c++17 -ftrapv -g -o"
CHECKMEM="valgrind"
OPTIONS_CHECKMEM="--leak-check=full --track-origins=yes --show-leak-kinds=all"
clean=0
if [ ! -z $1 ]
then
    if [ $1 = "clean" ]
    then
        rm -rf $OBJDIR
        rm -rf $TESTR
        if [ -d lcov ]
        then
            rm -rf ./lcov
        fi
        clean=1
    fi
fi
if [ $clean = 0 ]
then
    if [ -e $SW ]
    then
        if [ ! -e $OBJDIR ]
        then
            mkdir $OBJDIR
        fi
        if [ $SW -nt $PW ]
        then
            echo -e "\e[32m Lexer compiling \e[0m"
            $CC $SW -fprofile-arcs -ftest-coverage -c $OPTIONS_CC $OW --coverage
            $CC $OW -fprofile-arcs -ftest-coverage $OPTIONS_CC $PW --coverage
        fi
        dir=0
    else
        echo -e "\e[31m You are in wrong directory!!! \e[0m"
        dir=1
    fi
    if [ $dir = 0 ]
    then
        if [ -e $PW ]
        then
            if [ ! -z $1 ]
            then
                if [ $1 = "run" ]
                then
                    echo -e "\e[32m Lexer testing \e[0m"
                    if [ ! -z $2 ]
                    then
                        if [ $2 = "l" ]
                        then
                            if [ -d ./lcov ]
                            then
                                echo -e "\e[32m lcov have been already installed \e[0m"
                            else
                                echo -e "\e[32m installing lcov \e[0m"
                                git clone https://github.com/linux-test-project/lcov.git
                           fi
                        fi
                    fi
                    if [ ! -e $TESTR ]
                    then
                        mkdir $TESTR
                    fi
                    mkdir $TESTRES
                    mkdir $TESTVAL
                    if [ ! -z $2 ]
                    then
                        ./lcov/bin/lcov --directory $OBJDIR --zerocounters
                    fi
                    num=1
                    while [ -e $TESTDIR/Lex_test$num ]
                    do
                       $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test$num >$TESTRES/lex$num.txt 2>$TESTVAL/val$num.txt
                       num=$[ $num + 1 ]
                    done
                if [ ! -z $2 ]
                then
                    if [ $2 = "lcov" ]
                    then
                        ./lcov/bin/lcov --directory $OBJDIR/ --capture --rc lcov_branch_coverage=1 --output-file $OBJDIR/coverage.info
                        ./lcov/bin/genhtml $OBJDIR/coverage.info --branch-coverage  -o $OBJDIR/
                        if [ ! -z $3 ]
                        then
                            if [ $3 = "web" ]
                            then
                                firefox $OBJDIR/index.html
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi
  fi
fi
