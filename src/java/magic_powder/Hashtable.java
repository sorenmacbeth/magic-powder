package magic_powder;

public class Hashtable {
    static {
        System.out.println("Loading C library from Java!");
        // I could not get System.loadLibrary to work correctly with a .so file.
        // The downside to this is that you need to have the full, rooted path to the shared library.
        // Suggestions or tips are welcome!
        System.load(System.getProperty("user.dir") + "/src/c/libhashtable.so");
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
