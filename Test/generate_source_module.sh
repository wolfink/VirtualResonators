#!/bin/sh

JUCE_VERSION=$1
DEPENDENCIES_PATH=$2
SOURCE_PATH=$3
MODULE_PATH=$4
HEADER_PATH=$MODULE_PATH/source_code/source_code.h
CPP_PATH=$MODULE_PATH/source_code/source_code.cpp

rm -r $MODULE_PATH/source_code

mkdir $MODULE_PATH/source_code
touch $HEADER_PATH
touch $CPP_PATH

# Generate source_code.h
echo "* Generating source_code.h..."
echo "#pragma once" >> $HEADER_PATH

# Create juce module declaration
echo "** Creating JUCE module declaration..."
echo "/*" >> $HEADER_PATH
echo "BEGIN_JUCE_MODULE_DECLARATION" >> $HEADER_PATH
echo "ID: source_code" >> $HEADER_PATH
echo "vendor: wolfi" >> $HEADER_PATH
echo "version: $JUCE_VERSION" >> $HEADER_PATH
echo "name: Virtual Resonators Source Code" >> $HEADER_PATH
echo "description: Helper module for testing source code with JUCE unit tests" >> $HEADER_PATH

# Generate dependencies
echo "** Generating dependencies..."
DEPENDENCIES=$(ls $DEPENDENCIES_PATH | sed /juce_audio_plugin_client/d | tr '\n' ", ")
echo -n "dependencies: " >> $HEADER_PATH
echo -n $DEPENDENCIES >> $HEADER_PATH

sed -i "s/dependencies:.*/dependencies: ${DEPENDENCIES}source_code/g" $MODULE_PATH/../VirtualResonatorsTest.h

echo "
END_JUCE_MODULE_DECLARATION" >> $HEADER_PATH
echo "*/" >> $HEADER_PATH

# Generate header content, first include dependencies
# files included by other files go on top
echo "** Generating includes..."
IFS=',' read -r -a dependency_list <<< "$DEPENDENCIES"
for dependency in ${dependency_list[@]}; do
    echo "#include <$dependency/$dependency.h>" >> $HEADER_PATH
done
echo "using namespace juce;" >> $HEADER_PATH
generate_source_includes(){
    FILES=$@
    for file in ${FILES[@]}; do
        includes=($(awk '/include "/{print $2}' $SOURCE_PATH/$file | tr '\n' ' ' | tr -d '"'))
        if [ ${#includes[@]} != 0 ]; then
            (generate_source_includes $includes)
        fi
        echo '#include "'Source/$file'"' >> $HEADER_PATH
    done
}

FILES_ENTER=($(ls $SOURCE_PATH/*.h | sed s/"$SOURCE_PATH\/"//))
generate_source_includes ${FILES_ENTER[@]}

# Remove duplicate includes
tmpfile=$(mktemp)
cp $HEADER_PATH "$tmpfile" &&
awk '!a[$0]++' "$tmpfile" > $HEADER_PATH
rm $tmpfile

# Generate source_code.cpp
echo "* Generating source_code.cpp..."
echo "#include \"source_code.h\"" >> $CPP_PATH
for file in $SOURCE_PATH/*.cpp; do
    name="Source${file#$SOURCE_PATH}"
    echo -n "#include \"" >> $CPP_PATH
    echo -n $name >> $CPP_PATH
    echo "\"" >> $CPP_PATH
done

echo "* Copying Source folder into Test/modules/source_code"
mkdir $MODULE_PATH/source_code/Source
cp $SOURCE_PATH/* $MODULE_PATH/source_code/Source

for file in $MODULE_PATH/source_code/Source/*; do
    echo "* Editing $file"
    if [[ $file == *"PluginProcessor.h"* ]]; then
        sed -i '1s/^/#define JucePlugin_Name "VirtualResonators"\n/' $file
    fi
    sed -i /'#'"pragma once"/d $file
    sed -i /"include"/d $file
done