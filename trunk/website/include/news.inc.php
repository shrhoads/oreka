<?php
/*
 * Copyright 2004 Matt Brubeck
 * This file is licensed under a Creative Commons license:
 * http://creativecommons.org/licenses/by/2.0/
 */
class NewsItem {
  var $id;
  var $date;
  var $title;
  var $body;

  // Constructor.
  function NewsItem($date, $title, $body) {
    $this->date = $date;
    $this->title = $title;
    $this->body = $body;
  }

  // Returns the date, in the preferred format for the current locale.
  function dateStr() {
    // i18n-hint: Controls how dates are formatted.
    // See http://www.php.net/manual/function.strftime.php for details.
    return locale_to_unicode(strftime(_("%B %d, %Y"), $this->date));
  }
}

$news_items = array();
function add_news_item($dateStr, $id, $title, $body) {
  global $news_items;
  $date = strtotime($dateStr);
  $key = strftime("%Y-%m-%d", $date)."/".$id;
  $news_items[$key] = new NewsItem($date, $title, $body);
}
function most_recent_news_item() {
}

add_news_item(
  "October 26, 2005",
	"0.1-release",
  _("Oreka 0.1 Released"),
  _('<p>This is the first public release of oreka. Please check it out and let us know issues you are encountering on oreka-devel and oreka-user mailing lists.</p>')
);
?>
