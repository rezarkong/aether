library ieee;
use ieee.std_logic_1164.all;

library work;
use work.all;
use work.enae_pkg.all;

entity ctgen0 is
    port (s    : in  state0_t_arr;
          data : in  std_logic_vector(127 downto 0);
          ct   : out std_logic_vector(127 downto 0));
end entity;

architecture parallel of ctgen0 is
    
    signal f0_pt : std_logic_vector(127 downto 0);
    signal f0_ct : std_logic_vector(127 downto 0);

    signal x0 : std_logic_vector(127 downto 0);

begin

    x0 <= data;

    f0_pt <= s(0) xor s(1);
    f0_gen : entity f port map (f0_pt, f0_ct);
    ct <= f0_ct xor s(3) xor x0;

end architecture;

