package com.example.jnidemo; // Defines the package name (must match the JNI function prefix)

import androidx.appcompat.app.AppCompatActivity; // Import for standard Android Activity support
import android.os.Bundle; // Import for the saved state bundle
import android.widget.TextView; // Import for the UI text component

// Define the main class that extends the standard Android Activity
public class MainActivity extends AppCompatActivity {

    // 1. Declare the native methods (tells Java these are implemented in C++)
    // Returns a Java String, takes no parameters
    public native String helloFromJNI();
    // Returns a Java int, takes one Java int as a parameter
    public native int factorial(int n);
    // Returns a Java String, takes a Java String as a parameter
    public native String reverseString(String s);
    // Returns a Java int, takes an array of ints as a parameter
    public native int sumArray(int[] values);

    // 2. Load the native library
    static {
        // Loads "libnative-lib.so" at runtime (omit the 'lib' prefix and '.so' suffix)
        System.loadLibrary("native-lib");
    }

    // 3. Android Activity Lifecycle: Called when the app first starts
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState); // Call the parent class's onCreate logic
        setContentView(R.layout.activity_main); // Set the UI layout defined in XML

        // Initialize UI components by finding their IDs from the XML layout
        TextView tvHello = findViewById(R.id.tvHello); // Reference for the Hello JNI text
        TextView tvFact = findViewById(R.id.tvFact); // Reference for the factorial result text
        TextView tvReverse = findViewById(R.id.tvReverse); // Reference for the reversed string text
        TextView tvArray = findViewById(R.id.tvArray); // Reference for the array sum text

        // --- 1. Test Hello World ---
        // Call the C++ helloFromJNI() and set its result directly to the TextView
        tvHello.setText(helloFromJNI());

        // --- 2. Test Factorial ---
        // Pass the integer 10 to the C++ factorial function
        int fact10 = factorial(10);
        // Check if the result is valid (non-negative)
        if (fact10 >= 0) {
            // Update UI with the calculated factorial value
            tvFact.setText("Factoriel de 10 = " + fact10);
        } else {
            // Update UI with the error code returned by the C++ logic
            tvFact.setText("Erreur factoriel, code = " + fact10);
        }

        // --- 3. Test String Reversal ---
        // Pass a string to C++, get the reversed version back
        String reversed = reverseString("JNI is powerful!");
        // Display the reversed string in the UI
        tvReverse.setText("Texte inverse : " + reversed);

        // --- 4. Test Array Sum ---
        // Initialize an array of integers in Java
        int[] numbers = {10, 20, 30, 40, 50};
        // Pass the array object to the C++ sumArray function
        int sum = sumArray(numbers);
        // Display the sum result returned from the native side
        tvArray.setText("Somme du tableau = " + sum);
    }
}