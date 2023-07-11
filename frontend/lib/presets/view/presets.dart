import 'package:flutter/material.dart';

import '../model/presets.dart';

class Presets extends StatelessWidget {
  const Presets({
    super.key,
    required this.createPreset,
    required this.savePreset,
    required this.deletePreset,
    required this.undo,
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
  final void Function()? undo;
  final void Function()? revertPreset;
  final void Function()? selectPreviousPreset;
  final void Function()? selectNextPreset;
  final void Function(PresetState preset)? selectPreset;
  final List<PresetState>? presets;
  final PresetState? selectedPreset;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            IconButton(
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
              icon: const Icon(Icons.undo),
              tooltip: 'Undo',
              onPressed: undo,
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
                child: DropdownButton<PresetState>(
                  isExpanded: true,
                  value: selectedPreset,
                  items: presets
                      ?.map(
                        (e) => DropdownMenuItem(
                          value: e,
                          child: Text(e.name),
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
