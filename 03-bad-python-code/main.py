def girouette_spagett(R1=1000, Vin=3.3, inverted=False):
    """
    The wind direction sensor implementation we got for the weather station is
    kinda garbage. This code tries to find what analog voltages I should expect
    for each wind direction without doing 16 fucking voltage dividers by hand...

    Input values come from the PCB schematic Serge did. I'm bad at reading schematics
    so I made sure I was prepared:
    - R1:  Resistance between Vin and Vout
    - Vin: Input voltage
    - inverted: Flips R1 and R2 just in case I read the PCB wrong
    """
    # fmt: off
    directions = [
        # Deg.      Res.
        (0,      33_000),
        (22.5,    6_570),
        (45,      8_200),
        (67.5,      891),
        (90,      1_000),
        (112.5,     688),
        (135,     2_200),
        (157.5,   1_410),
        (180,     3_900),
        (202.5,   3_140),
        (225,    16_000),
        (247.5,  14_120),
        (270,   120_000),
        (292.5,  42_120),
        (315,    64_900),
        (337.5,  21_880),
    ]
    # fmt: on
    for i in range(len(directions)):
        theta = directions[i][0]
        direction_R2 = directions[i][1]
        r1 = R1
        r2 = direction_R2
        if inverted:
            r1, r2 = r2, r1
        Vout = Vin * (r2 / (r1 + r2))
        # ready to copy/paste in the C code
        print(f"{{ {theta:6.1f}, {Vout:6.3f} }},")


def main():
    girouette_spagett()


if __name__ == "__main__":
    main()
