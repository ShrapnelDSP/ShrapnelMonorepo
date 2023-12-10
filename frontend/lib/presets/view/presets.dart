/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

import 'package:flutter/material.dart';

import '../model/presets.dart';

class Presets extends StatelessWidget {
  const Presets({
    super.key,
    required this.createPreset,
    required this.savePreset,
    required this.deletePreset,
    required this.revertPreset,
    required this.selectPreviousPreset,
    required this.selectNextPreset,
    required this.selectPreset,
    required this.presets,
    required this.selectedPreset,
  });

  final void Function(String name)? createPreset;
  final void Function()? savePreset;
  final void Function()? deletePreset;
  final void Function()? revertPreset;
  final void Function()? selectPreviousPreset;
  final void Function()? selectNextPreset;
  final void Function(PresetRecord preset)? selectPreset;
  final List<PresetRecord>? presets;
  final PresetRecord? selectedPreset;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            IconButton(
              key: const Key('presets-create-button'),
              icon: const Icon(Icons.add),
              tooltip: 'Create new preset',
              onPressed: () async {
                final name = await showDialog<String>(
                  context: context,
                  builder: (_) {
                    return const PresetNameDialog();
                  },
                );
                if (name != null) {
                  createPreset!(name);
                }
              },
            ),
            IconButton(
              key: const Key('presets-save-button'),
              icon: const Icon(Icons.save),
              tooltip: 'Save preset',
              onPressed: savePreset,
            ),
            IconButton(
              icon: const Icon(Icons.delete),
              tooltip: 'Delete preset',
              onPressed: deletePreset,
            ),
            IconButton(
              icon: const Icon(Icons.restart_alt),
              tooltip: 'Revert changes',
              onPressed: revertPreset,
            ),
          ],
        ),
        Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            IconButton(
              icon: const Icon(Icons.chevron_left),
              tooltip: 'Previous preset',
              onPressed: selectPreviousPreset,
            ),
            SizedBox(
              width: 150,
              child: ButtonTheme(
                alignedDropdown: true,
                child: DropdownButton<PresetRecord>(
                  key: const Key('presets-current-preset-name'),
                  isExpanded: true,
                  value: selectedPreset,
                  items: presets
                      ?.map(
                        (e) => DropdownMenuItem(
                          value: e,
                          child: Text(e.preset.name),
                        ),
                      )
                      .toList(),
                  onChanged: (selected) => selectPreset?.call(selected!),
                ),
              ),
            ),
            IconButton(
              icon: const Icon(Icons.chevron_right),
              tooltip: 'Next preset',
              onPressed: selectNextPreset,
            ),
          ],
        ),
      ],
    );
  }
}

class PresetNameDialog extends StatefulWidget {
  const PresetNameDialog({super.key});

  @override
  State<StatefulWidget> createState() {
    return PresetNameDialogState();
  }
}

class PresetNameDialogState extends State<PresetNameDialog> {
  final _formKey = GlobalKey<FormState>();

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
      icon: const Icon(Icons.drive_file_rename_outline_rounded),
      title: const Text('Name Preset'),
      content: Form(
        key: _formKey,
        child: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              TextFormField(
                key: const Key('presets-name-field'),
                decoration: const InputDecoration(
                  label: Text('Name'),
                  hintText: 'New Preset',
                ),
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'can not be empty';
                  }

                  return null;
                },
                onSaved: (newValue) {
                  Navigator.of(context).pop(newValue);
                },
              ),
            ],
          ),
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: const Text('Cancel'),
        ),
        FilledButton(
          key: const Key('presets-confirm-name-button'),
          onPressed: () {
            final isValid = _formKey.currentState?.validate() ?? false;
            if (isValid) {
              _formKey.currentState?.save();
            }
          },
          child: const Text('Save'),
        ),
      ],
    );
  }
}
