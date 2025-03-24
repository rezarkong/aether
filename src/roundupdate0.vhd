library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity roundupdate0 is
    port (si : in  state0_t_arr;
          so : out state0_t_arr;

          x0 : in std_logic_vector(127 downto 0));
end entity;

architecture structural of roundupdate0 is
    signal f0, f1, f2, f3 : std_logic_vector(127 downto 0);
begin

    f0_gen : entity f port map (si(3), f0);
    f1_gen : entity f port map (si(0), f1);
    f2_gen : entity f port map (si(1), f2);
    f3_gen : entity f port map (si(2), f3);

    so(0) <= f0 xor x0;
    so(1) <= f1 xor si(1);
    so(2) <= f2 xor si(3);
    so(3) <= f3 xor si(0);

end architecture;
