//  these functions need to be available to inline scripts
var metaEditor = (function ($) {
  //  get metadata needed for DB transaction.  Some of which is not attached directly to element being worked on but to the row
  var editor = {
    getDataFromEl: function(el, tag, val) {
      var val = (typeof val !== 'undefined') ? val : $(el).find(tag).val(),
      tag = tag ? tag : 'input',
      data = {
        value: val,
        colmn: $(el).attr('data-colmn'),
        datasetID: $(el).parent().attr('data-datasetID')
      };
      if ($(el).parent().attr('data-dsid')) { data.dsid = $(el).parent().attr('data-dsid'); }
      if ($(el).parent().attr('data-fid')) { data.fid = $(el).parent().attr('data-fid'); }
      if ($(el).attr('data-oid')) { data.oid = $(el).attr('data-oid'); }

      return data;
    },
    sendData: function(data, el, callback) {
      //  move from editing to saving
      $(el).removeClass('editing').addClass('saving');
      //  save to db
      $('#results').load('/admin/datasets/override', data, function(){
        // done with saving
        $(el).removeClass('saving');
        // update dom
        $(el).attr('data-value', data.value);
        // run callback
        if (callback) {
          callback();
        }
      });
    },
    initEdit: function(el) {
      var val = $(el).html(),                                             //  field value
      tag = $(el).attr('data-tag') ? $(el).attr('data-tag') : 'input',    //  html tag type, if not input
      type = $(el).attr('data-type') ? $(el).attr('data-type') : 'text';  //  input type

      // check for exiting input - if exists, exit
      if ($(el).find(tag).length > 0 ) {
        return false;
      }

      // we're now editing
      $(el).addClass('editing');
      //  store value
      $(el).attr('data-value', val);
      //  replace static value with input
      input = $('<'+tag+'/>',{
        type: type,
        value: val
      //  on close go back to static text and save to database
      }).blur(function(){
        // map data from DOM
        var data = editor.getDataFromEl(el, tag);
        // save and update DOM
        editor.sendData(data, el, function(){
          $(el).html(data.value);
        });
      //  prevent focus from bubbling up to table cell and causing endless loop
      }).focus(function(e){
        e.stopPropagation();
      });
      //  set focus on newly created input
      $(el).html('').append(input).find(tag).focus();
    },
    rowResetListener: function(e) {
      var row = $(e.target).parent().parent(),
      overrides = $(row).find('td[data-oid]');

      $.each(overrides, function(n, override){
        var data, val = $(override).html();
        //  get all needed column values
        data = editor.getDataFromEl(override, 'input', val);
        //  mark for deletion
        data.delete = true;
        // save and update DOM
        editor.sendData(data, override);
      });
    }
  };
  $(document).ready(function($) {
    //  replace editable boolean values with checkboxes that update value
    $('td.editable.boolean').each(function(index, el) {
      var val = Number($(el).html()),
      box = $('<input/>',{
        type: 'checkbox',
        checked: Boolean(val),
        'data-value': val
      }).attr('checked', val ? true : false).change(function(){
        //  translate value to acceptable format for DB
        var val = ($(el).find('input').attr('checked')) ? 1 : 0,
        //  get all needed column values
        data = editor.getDataFromEl(el, 'input', val);
        // save and update DOM
        editor.sendData(data, el);
      });
      // attach to DOM
      $(el).html(box);
    });
    //  edit non-boolean values on double click and save on lose focus
    $('td.editable:not(.boolean)').focus(function(e) {
      var el = $(e.target);
      editor.initEdit(el);
    });
    //  reset to default values on overridable fields
    $('td.reset button.reset').click(editor.rowResetListener);
  });

  return editor;
})(jQuery);