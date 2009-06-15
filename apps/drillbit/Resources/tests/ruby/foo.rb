require 'bar'

class Foo
  def bar
    'hello world'
  end
  def yum
    bar = Bar.new
    bar.mitzvah
  end
  def what_is_love?(i_am)
    bar = Bar.new
    bar.give_me_some(i_am)
  end
end
