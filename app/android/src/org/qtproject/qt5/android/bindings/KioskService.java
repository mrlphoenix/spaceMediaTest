package org.qtproject.qt5.android.bindings;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;

import org.kde.necessitas.ministro.IMinistro;
import org.kde.necessitas.ministro.IMinistroCallback;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.PackageInfo;
import android.content.res.Configuration;
import android.content.res.Resources.Theme;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.view.accessibility.AccessibilityEvent;
import dalvik.system.DexClassLoader;
import android.view.ViewGroup;
import android.view.Gravity;
import android.graphics.PixelFormat;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.KeyguardManager;
import android.content.Context;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.app.Notification;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.os.IBinder;
import android.provider.SyncStateContract.Constants;
import android.app.ActivityManager;
import java.util.concurrent.TimeUnit;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import java.util.List;


public class KioskService extends Service {

  private static final long INTERVAL = TimeUnit.SECONDS.toMillis(1); // periodic interval to check in seconds -> 2 seconds
  private static final String TAG = KioskService.class.getSimpleName();
  private static final String PREF_KIOSK_MODE = "pref_kiosk_mode";

  private Thread t = null;
  private Context ctx = null;
  private boolean running = false;

  @Override
  public void onDestroy() {
    Log.i(TAG, "Stopping service 'KioskService'");
    running =false;
    super.onDestroy();
  }

  @Override
  public int onStartCommand(Intent intent, int flags, int startId) {
    Log.i("TeleDS", "Starting service 'KioskService'");
    running = true;
    ctx = this;

    // start a thread that periodically checks if your app is in the foreground
    t = new Thread(new Runnable() {
      @Override
      public void run() {
        do {
          handleKioskMode();
          try {
            Thread.sleep(INTERVAL);
          } catch (InterruptedException e) {
            Log.i(TAG, "Thread interrupted: 'KioskService'");
          }
        }while(running);
        stopSelf();
      }
    });

    t.start();
    return Service.START_NOT_STICKY;
  }

  private void handleKioskMode() {
      Log.i("TeleDS", "handleKioskMode");
    // is Kiosk Mode active?
     // if(isKioskModeActive(ctx)) {
          Log.i("TeleDS", "kioskModeActive");
        // is App in background?
      if(isInBackground()) {
          Log.i("TeleDS", "KioRestoreApp");
        restoreApp(); // restore!
      }
    //}
  }

  private boolean isInBackground() {
    ActivityManager am = (ActivityManager) ctx.getSystemService(Context.ACTIVITY_SERVICE);

    List<ActivityManager.RunningTaskInfo> taskInfo = am.getRunningTasks(1);
    ComponentName componentInfo = taskInfo.get(0).topActivity;
    return (!ctx.getApplicationContext().getPackageName().equals(componentInfo.getPackageName()));
  }

  private void restoreApp() {
    // Restart activity
    Intent i = new Intent(ctx, QtActivity.class);
    i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    ctx.startActivity(i);
  }

  public boolean isKioskModeActive(final Context context) {
    SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
    return sp.getBoolean(PREF_KIOSK_MODE, false);
  }

  @Override
  public IBinder onBind(Intent intent) {
    return null;
  }
}
