package magic_powder;

import java.io.*;

public class Hashtable {
    static {
        String path = "/libhashtable.so";

        try {
            File temp = File.createTempFile("libhashtable", ".so");
            temp.deleteOnExit();
            if (!temp.exists()) {
                throw new FileNotFoundException("File " + temp.getAbsolutePath() + " does not exist.");
            }

            byte[] buffer = new byte[1024];
            int readBytes;
            InputStream is = Hashtable.class.getResourceAsStream(path);
            if (is == null) {
                throw new FileNotFoundException("File " + path + " was not found inside JAR.");
            }
            OutputStream os = new FileOutputStream(temp);
            while ((readBytes = is.read(buffer)) != -1) {
                os.write(buffer, 0, readBytes);
            }
            os.close();
            is.close();

            System.out.println("Loading " + temp.getAbsolutePath());
            System.load(temp.getAbsolutePath());
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public static native long makeHashtable(int keySize, int valueSize,
                                            int capacity, int nbuckets,
                                            String filename);
    public static native long mapHashtable(String filename);
    public static native void unmapHashtable(long hashtable);

    public static native void insert_bytes(long hashtable, String key, byte[] value);
    public static native void insert_doubles(long hashtable, String key, double[] values);

    public static native byte[] get_bytes(long hashtable, String key);
    public static native double[] get_doubles(long hashtable, String key);
}
