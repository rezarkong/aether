library ieee;
use ieee.numeric_std.all;
use ieee.std_logic_1164.all;

entity controller is
    port (clk   : in std_logic;
          reset : in std_logic;

          last_block : std_logic;

          ad_empty  : std_logic;
          msg_empty : std_logic;

          sr_load_en : out std_logic;
          sr56_sel   : out std_logic;
          ru_aux_sel : out std_logic_vector(1 downto 0));
end entity;

architecture behavioural of controller is

    signal count_reset : std_logic;
    signal count_en    : std_logic;
    signal count       : unsigned(3 downto 0);
    
    type state_type is (load_state, init_state, ad_state, msg_state, tag_state);
    signal state, next_state : state_type;

begin

    counter : process(clk)
    begin
        if rising_edge(clk) then
            if count_reset = '1' then
                count <= "0000";
            elsif count_en = '1' and count < 9 then
                count <= count + 1;
            end if;
        end if;
    end process;
    
    state_reg : process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                state <= load_state;
            else 
               state <= next_state;
           end if;
        end if;
    end process state_reg;

    fsm : process(all)
    begin

        next_state <= state;

        count_reset <= '0';
        count_en    <= '0';
        
        sr_load_en <= '0';
        sr56_sel   <= '0';
        ru_aux_sel <= "00";

        case state is

            when load_state =>
                sr_load_en <= '1';
                count_reset <= '1';

                next_state <= init_state;

            when init_state =>
                next_state <= init_state;
                count_en   <= '1';

                if count = 9 then
                    sr56_sel    <= '1';
                    count_reset <= '1';
                    
                    if ad_empty = '0' then
                        next_state <= ad_state;
                    elsif msg_empty = '0' then
                        next_state <= msg_state;
                    else
                        next_state <= tag_state;
                    end if;
                end if;

            when ad_state =>
                next_state <= ad_state;
                ru_aux_sel <= "01";

                if last_block = '1' then
                    if msg_empty = '0' then
                        next_state <= msg_state;
                    else
                        next_state <= tag_state;
                    end if;
                end if;
            
            when msg_state =>
                next_state <= msg_state;
                ru_aux_sel <= "01";

                if last_block = '1' then
                    next_state <= tag_state;
                end if;


            when tag_state =>
                next_state <= tag_state;
                count_en   <= '1';
                ru_aux_sel <= "10";
        
        end case;
    end process;

end architecture;
