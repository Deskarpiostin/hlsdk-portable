A try to make dod client for android

## Android
1. Set up [Android Studio/Android SDK](https://developer.android.com/studio).

### Android Studio
Open the project located in the `android` folder and build.

### Command-line
```
cd android
./gradlew assembleRelease
```

### Customizing the build
settings.gradle:
* **rootProject.name** - project name displayed in Android Studio (optional).

app/build.gradle:
* **android->namespace** and **android->defaultConfig->applicationId** - set both to desired package name.
* **getBuildNum** function - set **releaseDate** variable as desired.

app/java/su/xash/hlsdk/MainActivity.java:
* **.putExtra("gamedir", ...)** - set desired gamedir.

src/main/AndroidManifest.xml:
* **application->android:label** - set desired application name.
* **su.xash.engine.gamedir** value - set to same as above.