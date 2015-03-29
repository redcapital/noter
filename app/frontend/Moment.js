.pragma library

function ago(timestamp) {
	var diff = Math.max(0, Date.now() / 1000 - timestamp)
	if (diff < 60) return 'moments ago'
	if (diff < 120) return '1 minute ago'
	if (diff < 3600) return Math.floor(diff / 60) + ' minutes ago'
	if (diff < 7200) return '1 hour ago'
	if (diff < 86400) return Math.floor(diff / 3600) + ' hours ago'
	var days = Math.floor(diff / 86400)
	if (days === 1) return 'Yesterday'
	if (days < 7) return days + ' days ago'
	if (days === 7) return '1 week ago'
	if (days < 31) return Math.ceil(days / 7) + ' weeks ago'
	if (days === 31) return '1 month ago'
	if (days < 365) return Math.ceil(days / 30) + ' months ago'
	return Math.floor(days / 365) + ' years ago'
}
