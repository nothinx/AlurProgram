# Compile + jalankan simulasi.cpp (kode library asli), lalu render grafik ke ../gambar/.
# Jalankan dari folder ini: python gambar.py   (butuh g++ dan matplotlib)
import csv, glob, io, os, subprocess, tempfile
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams.update({
    "figure.figsize": (8, 3.6), "figure.dpi": 100, "savefig.bbox": "tight", "savefig.pad_inches": 0.15,
    "figure.facecolor": "white", "axes.facecolor": "white", "savefig.facecolor": "white",
    "font.size": 10, "axes.titlesize": 11, "axes.titleweight": "bold", "axes.titlelocation": "left",
    "axes.spines.top": False, "axes.spines.right": False, "axes.edgecolor": "#9ca3af",
    "axes.grid": True, "grid.color": "#e5e7eb", "grid.linewidth": 0.8,
    "legend.frameon": False, "svg.fonttype": "path", "svg.hashsalt": "nothinx",
    "lines.linewidth": 1.8,
})
WARNA = {"utama": "#2563eb", "pembanding": "#dc2626", "ketiga": "#16a34a", "keempat": "#9333ea",
         "kelima": "#ea580c", "mentah": "#9ca3af", "target": "#111827"}
KELUAR = os.path.join("..", "gambar")


def koma(x, d=1):
    return f"{x:.{d}f}".replace(".", ",")


def simpan(fig, nama):
    fig.savefig(os.path.join(KELUAR, nama), format="svg", metadata={"Date": None})
    plt.close(fig)


def jalankan(sim, skenario):
    keluar = subprocess.run([sim, skenario], capture_output=True, text=True, check=True).stdout
    return list(csv.DictReader(io.StringIO(keluar)))


def pita(ax, potongan, y, tinggi, lebar_teks):
    """potongan: [(mulai, akhir, warna, teks)] digambar sebagai pita berwarna."""
    for a, b, warna, teks in potongan:
        ax.broken_barh([(a, b - a)], (y, tinggi), color=warna, lw=0)
        if teks and b - a > lebar_teks:
            ax.text((a + b) / 2, y + tinggi / 2, teks, ha="center", va="center", color="white", fontsize=9)


def grafik_lampu(baris):
    # pin contoh: 10 merah, 11 kuning, 12 hijau
    lampu = {"10": ("merah", WARNA["pembanding"]), "11": ("kuning", "#eab308"), "12": ("hijau", WARNA["ketiga"])}
    nyala = [(int(r["waktu"]) / 1000, r["pin"]) for r in baris]
    akhir = 25.0  # simulasi berjalan 25 detik
    batas = [t for t, _ in nyala] + [akhir]
    potongan = [(t, batas[i + 1], lampu[p][1], lampu[p][0]) for i, (t, p) in enumerate(nyala)]
    lama = {lampu[p][0]: batas[i + 1] - t for i, (t, p) in enumerate(nyala[:-1])}
    merah = [t for t, p in nyala if p == "10"]

    fig, ax = plt.subplots(figsize=(8, 2.3))
    pita(ax, potongan, 0, 1, 3)
    ax.plot([t for t, _ in nyala], [1.25] * len(nyala), "v", color=WARNA["target"], ms=6, clip_on=False)
    ax.text(nyala[1][0] + 0.15, 1.5, "▼ baruMasuk() true: sekali di awal tiap tahap", va="center", fontsize=9)
    ax.set_xlim(0, akhir)
    ax.set_ylim(0, 1.7)
    ax.set_yticks([])
    ax.spines["left"].set_visible(False)
    ax.grid(axis="y", visible=False)
    ax.set_xlabel("waktu (detik)")
    ax.set_title(f"Merah {koma(lama['merah'])} s → hijau {koma(lama['hijau'])} s → kuning {koma(lama['kuning'])} s, "
                 f"berulang tiap {koma(merah[1] - merah[0])} detik")
    simpan(fig, "lampu-lalu-lintas.svg")
    return lama, merah[1] - merah[0]


def grafik_penyiram(baris):
    data = [r for r in baris if r["jenis"] == "data"]
    t = [int(r["waktu"]) / 1000 for r in data]
    sensor = [float(r["nilai"]) for r in data]
    kosong = int(next(r for r in baris if r["jenis"] == "kosong")["waktu"]) / 1000
    mulai = [int(r["waktu"]) / 1000 for r in baris if r["jenis"] == "mulai"]
    berhenti = [(int(r["waktu"]) / 1000, r["nilai"]) for r in baris if r["jenis"] == "berhenti"]

    fig, (ax, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 4.2),
                                  gridspec_kw={"height_ratios": [4, 1], "hspace": 0.08})
    ax.axhline(600, color=WARNA["target"], ls="--", lw=1.2)
    ax.text(t[-1], 603, "batas kering 600", ha="right", va="bottom", color=WARNA["target"], fontsize=9)
    ax.axvline(kosong, color=WARNA["pembanding"], lw=1.2)
    ax.text(kosong + 1, 470, "tangki kosong", color=WARNA["pembanding"], va="bottom", fontsize=9)
    ax.plot(t, sensor, color=WARNA["utama"])
    lama = []
    for m, (b, alasan) in zip(mulai, berhenti):
        ax.axvspan(m, b, color=WARNA["utama"], alpha=0.15, lw=0)
        lama.append((b - m, alasan))
    (b0, _), (b1, _) = berhenti[0], berhenti[1]
    nilai = lambda w: min(zip(t, sensor), key=lambda d: abs(d[0] - w))[1]
    ax.annotate(f"tanah basah:\npompa mati setelah {koma(lama[0][0])} s", (b0, nilai(b0)), xytext=(14, 30),
                textcoords="offset points", fontsize=9, arrowprops={"arrowstyle": "-", "color": "#6b7280"})
    ax.annotate(f"masih kering:\npompa dimatikan di {koma(lama[1][0])} s", (b1, nilai(b1)), xytext=(24, -40), va="top",
                textcoords="offset points", fontsize=9, arrowprops={"arrowstyle": "-", "color": "#6b7280"})
    ax.set_ylabel("sensor tanah\n(besar = kering)")

    # pita tahap: MEMANTAU 0, MENYIRAM 1, MERESAP 2
    nama = [("MEMANTAU", WARNA["mentah"]), ("MENYIRAM", WARNA["utama"]), ("MERESAP", WARNA["ketiga"])]
    potongan, awal = [], 0
    for i in range(1, len(data) + 1):
        if i == len(data) or data[i]["tahap"] != data[awal]["tahap"]:
            warna_, teks = nama[int(data[awal]["tahap"])][1], nama[int(data[awal]["tahap"])][0]
            potongan.append((t[awal], t[i] if i < len(data) else t[-1], warna_, teks))
            awal = i
    pita(ax2, potongan, 0, 1, 25)
    ax2.set_yticks([0.5], ["tahap"])
    ax2.set_ylim(0, 1)
    ax2.grid(axis="y", visible=False)
    ax2.set_xlim(0, t[-1])
    ax2.set_xlabel("waktu (detik); biru = tahap MENYIRAM (pompa menyala)")
    ax.set_title(f"Pompa mati saat tanah basah ({koma(lama[0][0])} s), atau paling lama "
                 f"{koma(lama[1][0], 0)} s saat tangki kosong")
    simpan(fig, "penyiram-tanaman.svg")
    return lama


if __name__ == "__main__":
    os.makedirs(KELUAR, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        sim = os.path.join(tmp, "sim")
        subprocess.run(["g++", "-std=c++11", "-O2", "-I../test", "-I../../src", "simulasi.cpp",
                        *sorted(glob.glob("../../src/*.cpp")), "-o", sim], check=True)
        print("lampu:", grafik_lampu(jalankan(sim, "lampu")))
        print("penyiram:", grafik_penyiram(jalankan(sim, "penyiram")))
