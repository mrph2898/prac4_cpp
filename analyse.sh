#!/bin/bash
CC="g++-7"
OBJDIR="./Objfiles"
TESTDIR="./Tests/Analyse_tests"
TESTRES="./TestsResults/Analyse_results"
TESTVAL="./TestsResults/Analyse_valgrind"
SW="./SourceCode/parser.cpp"
OW="$OBJDIR/p.o"
PW="$OBJDIR/p"
OPTIONS_CC="-O2 -Wall -Werror -pedantic-errors -Wextra -std=c++17 -ftrapv -g -o"
CHECKMEM="valgrind"
OPTIONS_CHECKMEM="--leak-check=full --track-origins=yes --show-leak-kinds=all"
clean=0
if [ ! -z $1 ]
then
    if [ $1 = "clean" ]
    then
        ./test_lexer.sh clean
        clean=1
    fi
fi
if [ $clean = 0 ]
then
    ./test_lexer.sh run l
    if [ -e $SW ]
    then
        if [ $SW -nt $PW ]
        then
            echo -e "\e[32m Parser compiling... \e[0m"
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
                    echo -e "\e[32m Parser testing \e[0m"
                    if [ ! -e $TESTRES ]
                    then
                        mkdir $TESTRES
                        mkdir $TESTVAL
                    fi
                    num=1
                    echo -e "\e[32m It requires some time... \e[0m"
                    while [ -e $TESTDIR/Analyse_test$num ]
                    do
                        $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Analyse_test$num >$TESTRES/syn$num.txt 2>$TESTVAL/val$num.txt
                        num=$[ $num + 1 ]
                        if [ $num -eq 8 ]
                        then
                            echo -e "\e[32m 8 tests was passed \e[0m"
                        elif [ $num -eq 17 ]
                        then
                            echo -e "\e[32m 8 tests yet \e[0m"
                        fi
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
