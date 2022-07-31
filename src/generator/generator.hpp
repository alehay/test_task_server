#ifndef _GENERATOR_HPP_
#define _GENERATOR_HPP_

template <typename t>
class generator {
  generator() = delete;
  generator(generator& oth) = delete;
  generator(const generator& oth) = delete;
  generator(generator&& oth) = delete;
  generator operator=(const generator oth) = delete;
  generator operator=(generator&& oth) = delete;

  

}

#endif  //_GENERATOR_HPP_