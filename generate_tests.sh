#!/bin/sh

NAME=VirtualResonatorsTest
BUILD=VisualStudio2019
DIR=Test
MODE=Debug
RUN=${1:-0}
LOGS_DIR=Logs
GEN_DIR=$DIR/$NAME
BUILD_DIR=$GEN_DIR/Builds/$BUILD
EXEC_PATH=$BUILD_DIR/x64/$MODE/ConsoleApp/$NAME.exe

PROJUCER_PATH=D:/JUCE/Projucer.exe
PROJUCER_USER_MODULE_PATH=D:/Documents/JUCE/modules
MSBUILD_PATH="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/MSBuild.exe"

# Remove generated directory, if still present
rm -r $GEN_DIR 2> /dev/null

echo "Generating test project..."
# Create source module
./$DIR/generate_source_module.sh 6.1.4 JuceLibraryCode/modules Source $DIR/modules

# Generate test project
$PROJUCER_PATH --create-project-from-pip \
    $DIR/$NAME.h $DIR JuceLibraryCode/modules $DIR/modules

# Make project generate AppConfig.h (change to default setting)
sed -i /JUCERPROJECT/s/"useAppConfig=\"0\""// $GEN_DIR/$NAME.jucer

$PROJUCER_PATH --resave \
    $GEN_DIR/$NAME.jucer > $LOGS_DIR/test.log

# Set module paths manually
JCF_PATH="D:\/Documents\/JUCE\/juce-toys-master"
sed -i /"MODULEPATH id=\"jcf_debug\""/s/"path".*/"path=\"${JCF_PATH}\"\/>"/ $GEN_DIR/$NAME.jucer

#mv $DIR/AppConfig.h $GEN_DIR/JuceLibraryCode

# Run tests
if [ $RUN -eq 1 ]; then
    echo "Building tests..."
    # Build test project
    "$MSBUILD_PATH" -verbosity:quiet -property:WarningLevel=0 \
    $BUILD_DIR/$NAME.sln >> $LOGS_DIR/test.log

    # Execute test project
    if [ -f $EXEC_PATH ]; then
        ./$EXEC_PATH > $LOGS_DIR/results.log
    else
        echo "Build failed, check $LOGS_DIR/test.log"
    fi

    # Remove generated files
    rm -r $GEN_DIR
fi