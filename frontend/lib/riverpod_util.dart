import 'package:flutter/cupertino.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

extension ChangeNotifierWithCodeGenExtension on Ref {
  T listenAndDisposeChangeNotifier<T extends ChangeNotifier>(T notifier) {
    notifier.addListener(notifyListeners);
    onDispose(() => notifier.removeListener(notifyListeners));
    onDispose(notifier.dispose);
    return notifier;
  }
}
