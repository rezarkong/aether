library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity roundupdate2 is
    port (si : in  state2_t_arr;
          so : out state2_t_arr;

          x0 : in std_logic_vector(127 downto 0);
          x1 : in std_logic_vector(127 downto 0);
          x2 : in std_logic_vector(127 downto 0));
end entity;

architecture structural of roundupdate2 is
    signal f0, f1, f2, f3, f4, f5, f6, f7, f8 : std_logic_vector(127 downto 0);
begin

    f0_gen : entity f port map (si(8), f0);
    f1_gen : entity f port map (si(0), f1);
    f2_gen : entity f port map (si(1), f2);
    f3_gen : entity f port map (si(2), f3);
    f4_gen : entity f port map (si(3), f4);
    f5_gen : entity f port map (si(4), f5);
    f6_gen : entity f port map (si(5), f6);
    f7_gen : entity f port map (si(6), f7);
    f8_gen : entity f port map (si(7), f8);

    so(0) <= f0 xor x0;
    so(1) <= f1 xor x1;
    so(2) <= f2 xor si(2);
    so(3) <= f3 xor x2;
    so(4) <= f4 xor si(0);
    so(5) <= f5 xor si(1);
    so(6) <= f6 xor si(3);
    so(7) <= f7 xor si(8);
    so(8) <= f8 xor si(4);

end architecture;
