import unicodedata
import sys

def transform(txt):      
    normed = unicodedata.normalize('NFD', txt)
    txt = ''.join(c for c in normed if not unicodedata.combining(c))
    
    if any(not c.isalpha() for c in txt):
        return ""
    return txt

if __name__ == "__main__":
    print("Usage: exe [in] [out]")
    ifile = sys.argv[1]
    ofile = sys.argv[2]

    iff = open(ifile, "rt")
    off = open(ofile, "wt")
    
    all_lines = []
    for line in iff:
        t = transform(line[:-1])
        if len(t) > 0:
            all_lines.append(t)

    all_lines = set(all_lines)
    for l in all_lines:
        off.write(l + '\n');
