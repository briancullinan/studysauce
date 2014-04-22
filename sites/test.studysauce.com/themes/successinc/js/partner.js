
jQuery(document).ready(function($) {

    var uploader = new plupload.Uploader({
                                         alt_field: 0,
                                         browse_button: "partner-plupload-select",
                                         chunk_size: "512K",
                                         container: "partner-plupload",
                                         dragdrop: true,
                                         drop_element: "partner-plupload-filelist",
                                         filters: [
                                             {
                                                 extensions: "png,gif,jpg,jpeg,images",
                                                 title: "Allowed extensions"
                                             }
                                         ],
                                         flash_swf_url: "/sites/all/libraries/plupload/js/plupload.flash.swf",
                                         image_style: "achievement",
                                         image_style_path: "/sites/studysauce.com/files/styles/achievement/temporary/",
                                         max_file_size: "512MB",
                                         max_files: 1,
                                         multipart: false,
                                         multiple_queues: true,
                                         name: "field_goals[und][0][field_photo_evidence][und]",
                                         runtimes: "html5,gears,flash,silverlight,browserplus,html4",
                                         silverlight_xap_url: "/sites/all/libraries/plupload/js/plupload.silverlight.xap",
                                         title_field: 0,
                                         unique_names: true,
                                         upload: "partner-plupload-upload",
                                         url: "/plupload/119?plupload_token=0vZpRVu7qCivCE4lJvc9DZUqwsdq3IU3dgn8-afhL0U",
                                         urlstream_upload: false
                                     });
    $('#partner-plupload-select').click(function(e) {
        uploader.start();
        e.preventDefault();
    });
    uploader.init();

});

