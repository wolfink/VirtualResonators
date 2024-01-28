#pragma once

#include "../PluginProcessor.h"
#include "../VirtualResonators.h"
#include "VRComponent.h"

class PresetControl : public VirtualResonatorsComponent<>
{
    VRShapeButton _left_btn, _right_btn;
    TextButton    _save_btn, _delete_btn;
    ComboBox      _preset_name;
    File          _directory;

    VirtualResonatorsAudioProcessor& _audioProcessor;

public:

    PresetControl(VirtualResonatorsAudioProcessor &a) :
    _audioProcessor(a)
    {
        _directory = File::getSpecialLocation(File::SpecialLocationType::currentExecutableFile)
            .getParentDirectory()
            .getParentDirectory()
            .getChildFile("Resources")
            .getChildFile("Presets");

        Path preset_btn_shape;
        preset_btn_shape.addTriangle(0, 10, 20, 20, 20, 0);

        auto preset_btn_color = _lookAndFeel
            .getCurrentColourScheme()
            .getUIColour(VRLookAndFeel::ColourScheme::UIColour::outline);

        configShapeButton(_left_btn , "LeftPre" , preset_btn_shape, preset_btn_color);
        _left_btn.onClick = [this] { prev_preset(); };

        preset_btn_shape.applyTransform(AffineTransform::rotation(MathConstants<float>::pi));
        configShapeButton(_right_btn, "RightPre", preset_btn_shape, preset_btn_color);
        _right_btn.onClick = [this] { next_preset(); };

        configComboBox(_preset_name, get_preset_list());
        _preset_name.setText("Init");
        _preset_name.setEditableText(true);
        _preset_name.onChange = [this] { load_preset(); };
        save_preset();

        configTextButton(_save_btn, "Save");
        _save_btn.onClick = [this] { save_preset(); };
    }

    void resized() override
    {
        FlexBox preset_bar;
        auto preset_height = 30;
        auto btn_size = preset_height * 0.8;

        preset_bar.alignItems = FlexBox::AlignItems::center;
        preset_bar.items.add(FlexItem(_left_btn)
            .withWidth(btn_size)
            .withHeight(btn_size)
            .withMargin(FlexItem::Margin(0, 5, 0, 0)));
        preset_bar.items.add(FlexItem(_preset_name)
            .withFlex(1)
            .withHeight(preset_height)
            .withMargin(FlexItem::Margin(0, 5, 0, 0)));
        preset_bar.items.add(FlexItem(_right_btn)
            .withWidth(btn_size)
            .withHeight(btn_size));
        preset_bar.items.add(FlexItem(_save_btn)
            .withFlex(0.25)
            .withHeight(preset_height)
            .withMargin(FlexItem::Margin(0, 0, 0, 10)));
        preset_bar.performLayout(getLocalBounds());
    }

private:
    void prev_preset()
    {
        int prev_id = _preset_name.getSelectedId() - 1;
        prev_id = (prev_id < 1) ? _preset_name.getNumItems() : prev_id;
        _preset_name.setSelectedId(prev_id);
    }

    void next_preset()
    {
        int next_id = _preset_name.getSelectedId() + 1;
        next_id = (next_id > _preset_name.getNumItems()) ? 1 : next_id;
        _preset_name.setSelectedId(next_id);
    }

    void load_preset()
    {
        File file = File(_directory.getFullPathName() + SLASH + _preset_name.getText() + ".vrpst");

        // If file exist, load it, otherwise refresh the item list for the combo box
        if (file.exists()) {
            String preset_data = file.loadFileAsString();
            _audioProcessor.setStateInformation((void*) preset_data.toRawUTF8(), preset_data.length());
        }
        else {
            _preset_name.clear();
            _preset_name.addItemList(get_preset_list(), 1);
        }
    }

    void save_preset()
    {
        if (_preset_name.getText().length() < 1) return;

        MemoryBlock state_info;
        _audioProcessor.getStateInformation(state_info);
        String preset_data = String((char*)state_info.getData(), state_info.getSize());
        String file_name = _directory.getFullPathName() + SLASH + _preset_name.getText() + ".vrpst";
        File file(file_name);

        if (!file.exists()) file.create();

        file.replaceWithText(preset_data);
        _preset_name.clear(dontSendNotification);
        _preset_name.addItemList(get_preset_list(), 1);
    }

    StringArray get_preset_list()
    {
        Array<File> files;
        StringArray file_names;
        _directory.findChildFiles(files, File::TypesOfFileToFind::findFiles, true, "*.vrpst");
        for (File f : files) file_names.add(f.getFileNameWithoutExtension());
        return file_names;
    }
};
