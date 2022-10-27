import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../../parameter.dart';

class MidiMappingPage extends StatelessWidget {
  const MidiMappingPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('MIDI Mapping'),
      ),
      body: DataTable(
        columns: const [
          DataColumn(label: Text('MIDI channel')),
          DataColumn(label: Text('CC number')),
          DataColumn(label: Text('Parameter')),
        ],
        rows: const [
          DataRow(
            cells: [
              DataCell(MidiChannelDropdown()),
              DataCell(MidiCCDropdown()),
              DataCell(ParametersDropdown()),
            ],
          ),
        ],
      ),
    );
  }
}

class MidiChannelDropdown extends StatelessWidget {
  const MidiChannelDropdown({super.key});

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        16,
        (i) => DropdownMenuItem(value: i + 1, child: Text('${i + 1}')),
      ),
      onChanged: (_) {},
      value: 1,
    );
  }
}

class MidiCCDropdown extends StatelessWidget {
  const MidiCCDropdown({super.key});

  @override
  Widget build(BuildContext context) {
    return DropdownButton(
      items: List<DropdownMenuItem<int>>.generate(
        128,
        (i) => DropdownMenuItem(value: i, child: Text('$i')),
      ),
      onChanged: (_) {},
      value: 1,
    );
  }
}

class ParametersDropdown extends StatelessWidget {
  const ParametersDropdown({super.key});

  @override
  Widget build(BuildContext context) {
    final parameters = context.read<ParameterService>().parameterNames;
    return DropdownButton<String>(
      items: [
        const DropdownMenuItem(child: Text('None')),
        ...parameters.map(
              (parameterName) => DropdownMenuItem(
                value: parameterName,
                child: Text(parameterName),
              ),
            )
      ],
      onChanged: (_) {},
    );
  }
}
