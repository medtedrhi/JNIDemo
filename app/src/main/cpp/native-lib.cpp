#include <jni.h>             // Core JNI header providing types (jstring, JNIEnv) and functions
#include <string>            // Standard C++ string library for string manipulation
#include <algorithm>         // Provides std::reverse for the string inversion function
#include <climits>           // Provides INT_MAX to check for integer overflows
#include <android/log.h>     // Android-specific header for logging to Logcat

#define LOG_TAG "JNI_DEMO"   // Define a tag name to filter logs in Android Studio's Logcat
// Macro to simplify calling the Android INFO log level
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
// Macro to simplify calling the Android ERROR log level
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// 1) Native "Hello World"
extern "C"                   // Prevents C++ name mangling so Java can find the function by name
JNIEXPORT jstring JNICALL    // JNIEXPORT makes the function visible in the shared library; jstring is the return type
Java_com_example_jnidemo_MainActivity_helloFromJNI( // Follows naming convention: Java_Package_Class_Method
        JNIEnv* env,         // Pointer to the JNI environment (interface to Java VM)
        jobject /* this */) { // Reference to the Java object calling this method (unused here)

    LOGI("Appel de helloFromJNI depuis le natif"); // Print an info message to Logcat
    return env->NewStringUTF("Hello from C++ via JNI !"); // Convert C-string to Java jstring and return it
}

// 2) Factorial with error handling
extern "C"                   // Ensure C linkage for the Java Virtual Machine
JNIEXPORT jint JNICALL       // Returns a jint (mapped to Java's int)
Java_com_example_jnidemo_MainActivity_factorial(
        JNIEnv* env,         // JNI interface pointer
        jobject /* this */,  // Reference to the calling Java object
        jint n) {            // The integer input passed from Java

    if (n < 0) {             // Check if the input is negative
        LOGE("Erreur : n negatif"); // Log an error message
        return -1;           // Return error code -1 to Java
    }

    long long fact = 1;      // Use a 64-bit integer to store intermediate factorial values
    for (int i = 1; i <= n; i++) { // Loop from 1 up to n
        fact *= i;           // Multiply current value by the iterator
        if (fact > INT_MAX) { // Check if the result exceeds the maximum value of a 32-bit int
            LOGE("Overflow detecte pour n=%d", n); // Log an overflow error
            return -2;       // Return error code -2 to Java
        }
    }

    LOGI("Factoriel de %d calcule en natif = %lld", n, fact); // Log the successful calculation
    return static_cast<jint>(fact); // Cast the 64-bit long long back to a 32-bit jint for Java
}

// 3) String Reversal: Java -> C++ -> Java
extern "C"                   // Linkage specification
JNIEXPORT jstring JNICALL    // Returns a jstring
Java_com_example_jnidemo_MainActivity_reverseString(
        JNIEnv* env,         // JNI environment
        jobject /* this */,  // Calling Java object
        jstring javaString) { // The string passed from Java

    if (javaString == nullptr) { // Check if the Java string object is null
        LOGE("Chaine nulle recue"); // Log error
        return env->NewStringUTF("Erreur : chaine nulle"); // Return an error string back to Java
    }

    // Convert Java jstring to a C-style char pointer (UTF-8)
    const char* chars = env->GetStringUTFChars(javaString, nullptr);
    if (chars == nullptr) {  // Check if memory allocation or conversion failed
        LOGE("Impossible de lire la chaine Java"); // Log error
        return env->NewStringUTF("Erreur JNI"); // Return error string
    }

    std::string s(chars);    // Create a standard C++ string object from the C-style string
    env->ReleaseStringUTFChars(javaString, chars); // IMPORTANT: Free the memory allocated for the C-style string

    std::reverse(s.begin(), s.end()); // Use C++ algorithm to flip the string in place

    LOGI("String inversee = %s", s.c_str()); // Log the reversed string
    return env->NewStringUTF(s.c_str()); // Convert the reversed C++ string back to a Java jstring
}

// 4) Summing an int[] array
extern "C"                   // Linkage specification
JNIEXPORT jint JNICALL       // Returns a jint
Java_com_example_jnidemo_MainActivity_sumArray(
        JNIEnv* env,         // JNI environment
        jobject /* this */,  // Calling Java object
        jintArray array) {   // The integer array passed from Java

    if (array == nullptr) {  // Check if the array object is null
        LOGE("Tableau nul"); // Log error
        return -1;           // Return error code
    }

    jsize len = env->GetArrayLength(array); // Get the length of the Java array
    // Get a direct pointer to the array elements (might create a copy or pin memory)
    jint* elements = env->GetIntArrayElements(array, nullptr);

    if (elements == nullptr) { // Check if accessing array elements failed
        LOGE("Impossible d'acceder aux elements du tableau"); // Log error
        return -2;           // Return error code
    }

    long long sum = 0;       // Use 64-bit sum to prevent overflow during addition
    for (jsize i = 0; i < len; i++) { // Iterate through the array using jsize (index type)
        sum += elements[i];  // Add each element to the running sum
    }

    // Release the pointer; the '0' mode means copy back changes to Java (if any) and free the buffer
    env->ReleaseIntArrayElements(array, elements, 0);

    if (sum > INT_MAX) {     // Final check for integer overflow
        LOGE("Overflow sur la somme"); // Log error
        return -3;           // Return error code
    }

    LOGI("Somme du tableau = %lld", sum); // Log the final sum result
    return static_cast<jint>(sum); // Return the sum as a 32-bit jint
}