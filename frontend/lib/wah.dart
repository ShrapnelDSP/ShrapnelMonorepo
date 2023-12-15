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
import 'package:provider/provider.dart';

import 'parameter.dart';
import 'stompbox.dart';

class WahModel extends StompboxModel {
  WahModel({required ParameterService parameterService})
      : parameters = [
          AudioParameterDoubleModel(
            groupName: _name,
            name: 'Position',
            id: 'wahPosition',
            parameterService: parameterService,
          ),
          AudioParameterDoubleModel(
            groupName: _name,
            name: 'Vocal',
            id: 'wahVocal',
            parameterService: parameterService,
          ),
        ],
        bypass = AudioParameterDoubleModel(
          groupName: _name,
          name: 'Bypass',
          id: 'wahBypass',
          parameterService: parameterService,
        );

  static const _name = 'Wah';

  @override
  String get name => _name;

  @override
  final AudioParameterDoubleModel bypass;

  @override
  List<AudioParameterDoubleModel> parameters;
}

class Wah extends StatelessWidget {
  const Wah({
    super.key,
    required this.full,
    required this.onTap,
  });

  final bool full;
  final void Function() onTap;

  @override
  Widget build(BuildContext context) {
    return Stompbox(
      model: context.read<WahModel>(),
      onCardTap: onTap,
      full: full,
      primarySwatch: Colors.purple,
    );
  }
}
