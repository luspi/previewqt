import os

# this filters out the data from the plugin files
def getData(fname):

    # we don't want to register PreviewQt for these types
    if "_pdf" in fname or "_video" in fname or "_resvg" in fname:
        return ()

    print(fname)

    f = open(f"../../cplusplus/files/plugins/{fname}")
    txt = f.read()

    ret = []

    # get the data structure
    if "magick" in fname or "libvips" in fname:
        parts = txt.split("candidateData = {")[1].split(";")[0].split("}}},")
    else:
        parts = txt.split("setData(")[1].split(";")[0].split("}}},")

    # the perceived type based on plugin
    perceivedtype = "image"
    if "_pdf" in fname:
        perceivedtype = "document"
    elif "libarchive" in fname:
        perceivedtype = "compressed"
    elif "video" in fname:
        perceivedtype = "video"

    for p in parts:

        # these are additional data, not a format
        if p.count("{") < 4:
            continue

        # this filters out parts that are not important here
        splt = p.split("{{")[1].split("},")
        if len(splt) != 3:
            print(splt)

        # superfluous characters
        chars_to_remove = "'\"{}\n"

        # remove characters and split lists
        a = str.maketrans("", "", chars_to_remove)
        desc = splt[0].translate(a).strip()
        endg = splt[1].translate(a).split(",")
        mimt = splt[2].translate(a).split(",")

        # strip entries
        endg = [e.strip() for e in endg]
        mimt = [m.strip() for m in mimt]

        # no empty string entry only
        if len(mimt) == 1 and mimt[0] == "":
            mimt = []

        # we don't register for archives except for comic books
        if perceivedtype == "compressed" and desc != "Comic book archive":
            continue

        # return data
        ret.append((desc, endg, mimt, ("document" if desc == "Comic book archive" else perceivedtype)))

    return ret

print("> Looping over all plugins and composing data...")

processed_endings = []

directory = os.fsencode("../../cplusplus/files/plugins/")
for file in os.listdir(directory):
    filename = os.fsdecode(file)
    if filename.endswith(".cpp") and filename != "pqc_fileplugin.cpp":

        dat = getData(filename)
        allendings = []

        for line in dat:

            for endg in line[1]:
                allendings.append(endg)

    else:
        continue

    print(f">> {filename}")
    if len(allendings) > 0:
        print(f"[\"{"\", \"".join(allendings)}\"]")
        print("")
