package me.generalsx.vulkanprobe;

import android.app.Activity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity
        implements SurfaceHolder.Callback {

    private TextView report;
    private boolean libraryLoaded = false;

    private native String nativeRun(Surface surface);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);

        report = new TextView(this);
        report.setTextSize(14);
        report.setText("Preparing Vulkan test...");

        SurfaceView surfaceView = new SurfaceView(this);
        surfaceView.getHolder().addCallback(this);

        layout.addView(surfaceView,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 180));

        ScrollView scrollView = new ScrollView(this);
        scrollView.addView(report);

        layout.addView(scrollView,
                new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.MATCH_PARENT, 0, 1));

        setContentView(layout);

        try {
            System.loadLibrary("vulkan_probe");
            libraryLoaded = true;
        } catch (UnsatisfiedLinkError error) {
            report.setText("Failed to load Vulkan test library:\n"
                    + error.getMessage());
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (!libraryLoaded) {
            return;
        }

        report.setText("Running Vulkan test...");

        new Thread(() -> {
            String result;

            try {
                result = nativeRun(holder.getSurface());
            } catch (Exception error) {
                result = "Test failed:\n" + error.toString();
            }

            final String finalResult = result;

            runOnUiThread(() -> report.setText(finalResult));
        }).start();
    }

    @Override
    public void surfaceChanged(
            SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }
}
