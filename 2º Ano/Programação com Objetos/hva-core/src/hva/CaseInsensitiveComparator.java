package hva;

import java.io.Serializable;
import java.util.Comparator;

public class CaseInsensitiveComparator implements Comparator<String>, Serializable {
    @Overrite
    public int compare(String s1, String s2) {
        return s1.compareToIgnoreCase(s2);
    }
}