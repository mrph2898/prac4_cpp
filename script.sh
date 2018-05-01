#!/bin/bash
CC="g++-7"
OBJDIR="./Objfiles"
TESTDIR="./Tests"
TESTRES="./TestsResults"
SW="./SourceCode/gr.cpp"
OW="$OBJDIR/gr.o"
PW="$OBJDIR/gr.out"
OPTIONS_CC="-O2 -Wall -Werror -pedantic-errors -Wextra -fsanitize=undefined -std=c++17 -ftrapv  -g -o"
CHECKMEM="valgrind"
OPTIONS_CHECKMEM="--leak-check=full --track-origins=yes --show-leak-kinds=all"
clean=0
if [ ! -z $1 ]
then
    if [ $1 = "clean" ]
    then
        rm -rf $OBJDIR
        rm -rf $TESTRES
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
            $CC $SW -fprofile-arcs -ftest-coverage -c $OPTIONS_CC $OW --coverage
            $CC $OW -fprofile-arcs -ftest-coverage  $OPTIONS_CC $PW --coverage
        fi
        dir=0
    else
        echo You are in wrong direction!!!
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
                    echo Lets begin!
                    if [ ! -z $2 ]
                    then
                        if [ $2 = "lcov" ]
                        then
                            if [ -d ./lcov ]
                            then
                                echo lcov was installed
                            else
                                echo installing lcov
                                git clone https://github.com/linux-test-project/lcov.git
                           fi
                        fi
                    fi
                    if [ ! -e $TESTRES ]
                    then
                        mkdir $TESTRES
                    fi
                    if [ ! -z $2 ]
                    then
                        ./lcov/bin/lcov --directory $OBJDIR --zerocounters
                    fi    
                    $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test1 >$TESTRES/lex1.txt 2>$TESTRES/val1.txt
                    $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test2 >$TESTRES/lex2.txt 2>$TESTRES/val2.txt
                    $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test3 >$TESTRES/lex3.txt 2>$TESTRES/val3.txt
                    $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test4 >$TESTRES/lex4.txt 2>$TESTRES/val4.txt
                    $CHECKMEM $OPTIONS_CHECKMEM $PW <$TESTDIR/Lex_test5 >$TESTRES/lex5.txt 2>$TESTRES/val4.txt
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
