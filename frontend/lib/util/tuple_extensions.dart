import 'package:tuple/tuple.dart';

extension MapEntryExt<T1, T2> on MapEntry<T1, T2> {
  Tuple2<T1, T2> toTuple2() {
    return Tuple2<T1, T2>(key, value);
  }
}

extension MapExt<T1, T2> on Map<T1, T2> {
  Tuple2<T1, T2> toTuple2()
  {
    if(length != 1)
    {
      throw ArgumentError('Input map must contain exactly one element.');
    }

    return entries.first.toTuple2();
  }
}

extension Tuple2Ext<T1, T2> on Tuple2<T1, T2> {
  MapEntry<T1, T2> toMapEntry() {
    return MapEntry<T1, T2>(item1, item2);
  }

  Map<T1, T2> toMap() {
    final map = Map<T1, T2>.fromEntries(List.filled(1, toMapEntry()));

    return map;
  }

  Tuple2<T1, T2> copyWith({T1? item1, T2? item2}) {
    return Tuple2(
      item1 ?? this.item1,
      item2 ?? this.item2,
    );
  }
}
